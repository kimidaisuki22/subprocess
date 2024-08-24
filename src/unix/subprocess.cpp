#include "child_pipe.h"
#include "subprocess/process_controller.h"
#include "subprocess/process_controller_with_io.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <signal.h>
#include <subprocess/subprocess.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace subprocess {

class UnixController : virtual public Controller {
private:
  pid_t pid_;
  mutable int exitStatus; // mutable allows modification in a const method
  mutable bool exitStatusChecked;

public:
  explicit UnixController(pid_t pid) : pid_(pid) {}

  bool has_start() const override {
    return pid_ != 0;
    // Implement based on pid validity
  }

  bool is_running() const override {
    if (pid_ < 1) {
      return false;
    }
    int status;
    pid_t result = waitpid(pid_, &status, WNOHANG);
    return result == 0;
  }

  bool has_stopped() const override {
    if (pid_ < 1) {
      return true; // Invalid PID means it's not running
    }

    if (!exitStatusChecked) {
      int status;
      pid_t result = waitpid(pid_, &status, WNOHANG);
      if (result == 0) {
        return false; // The process is still running
      } else if (result == pid_) {
        if (WIFEXITED(status)) {
          exitStatus = WEXITSTATUS(status);
        } else {
          // Handle other exit scenarios (e.g., signal termination)
          exitStatus = -1;
        }
        exitStatusChecked = true;
        return true; // The process has stopped
      } else {
        // Handle errors according to your needs
        return true; // Assuming an error means the process has stopped
      }
    }

    return exitStatusChecked; // Return the stored state if already checked
  }

  void stop() override {
    if (pid_ > 0) {
      kill(pid_, SIGTERM); // or SIGKILL to force kill
    }
  }

  void wait() override {
    if (pid_ > 0) {
      int status;
      waitpid(pid_, &status, 0);
      exitStatusChecked = true;
      exitStatus = status;
    }
  }

  int return_code() const override {
    if (!exitStatusChecked) {
      bool stopped = has_stopped(); // Check if the process has stopped and
                                    // update exitStatus
      (void)stopped;                // Unused variable warning prevention
    }
    return exitStatus; // Return the stored exit status
  }
  uint64_t pid() const override { return pid_; }
};
class UnixController_with_io : virtual public UnixController,
                               virtual public Controller_with_IO {
public:
  UnixController_with_io(pid_t pid, Child_pipe &&pipe)
      : UnixController(pid), pipe_(std::move(pipe)) {}
  Length read(void *buffer, size_t read_size) override {
    return ::read(pipe_.get_parent_read_fd(), buffer, read_size);
  }
  Length write(const void *buffer, size_t read_size) override {
    return ::write(pipe_.get_parent_write_fd(), buffer, read_size);
  }
  bool close_read() override { return pipe_.close_parent_read() == 0; }
  bool close_write() override { return pipe_.close_parent_write() == 0; }

private:
  Child_pipe pipe_{};
};

std::unique_ptr<Controller> create(const Create_info_simplest &info) {
  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    execlp(info.execute_name.c_str(), info.execute_name.c_str(), (char *)NULL);
    // If execlp returns, it must have failed.
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Handle error in fork
    return {};
  }

  // Parent process
  return std::make_unique<UnixController>(pid);
}
std::unique_ptr<Controller> create(const Create_info_extend &info) {
  pid_t pid = fork();
  if (pid == 0) {
    std::vector<std::string> args_copy;
    std::vector<char *> argv;
    args_copy.push_back(info.execute_name);
    for (auto &arg : info.args) {
      args_copy.push_back(arg);
    }
    for (auto &arg : args_copy) {
      argv.push_back(arg.data());
    }
    // Child process
    execvp(info.execute_name.c_str(), argv.data());
    // If execlp returns, it must have failed.
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Handle error in fork
    return {};
  }

  // Parent process
  return std::make_unique<UnixController>(pid);
}
std::unique_ptr<Controller_with_IO>
create_with_io(const Create_info_extend &info) {
  Child_pipe pipe;
  if (!pipe.is_ok()) {
    // failed to create pipe.
    return nullptr;
  }
  pid_t pid = fork();
  if (pid == 0) {
    pipe.apply_redict_for_child();
    pipe.close_all();
    std::vector<std::string> args_copy;
    std::vector<char *> argv;
    args_copy.push_back(info.execute_name);
    for (auto &arg : info.args) {
      args_copy.push_back(arg);
    }
    for (auto &arg : args_copy) {
      argv.push_back(arg.data());
    }
    // Child process
    execvp(info.execute_name.c_str(), argv.data());
    // If execlp returns, it must have failed.
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Handle error in fork
    return {};
  }
  // Parent process
  pipe.close_child_read();
  pipe.close_child_write();

  return std::make_unique<UnixController_with_io>(pid, std::move(pipe));
}
} // namespace subprocess
