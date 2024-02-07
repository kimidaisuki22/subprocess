#include "subprocess/process_controller.h"
#include <cstdint>
#include <subprocess/subprocess.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

namespace subprocess {

class UnixController : public Controller {
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

std::unique_ptr<Controller> create(const Create_info_simplest &info) {
  pid_t pid = fork();
  if (pid == 0) {
    // Child process
    execlp(info.execute_name.c_str(), info.execute_name.c_str(), (char *)NULL);
    // If execlp returns, it must have failed.
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Handle error in fork
  }

  // Parent process
  return std::make_unique<UnixController>(pid);
}

} // namespace subprocess
