#pragma once
#include <unistd.h>
#include <utility>
namespace subprocess {
class Child_pipe {
public:
  Child_pipe() {
    pipe_return_code_ = pipe(fd_);
    if (is_ok()) {
      pipe_return_code_ = pipe(fd_ + 2);
    }
    // maybe log on failed: if(!is_ok()) perror("Faile to creat pipe");
  }
  ~Child_pipe() { close_all(); }
  Child_pipe(const Child_pipe &) = delete;
  Child_pipe(Child_pipe &&rhs) {
    for (int i = 0; i < 4; i++) {
      closed_[i] = true;
    }
    std::swap(pipe_return_code_, rhs.pipe_return_code_);
    for (int i = 0; i < 4; i++) {
      std::swap(fd_[i], rhs.fd_[i]);
      std::swap(closed_[i], rhs.closed_[i]);
    }
  }
  Child_pipe &operator=(Child_pipe &&rhs) {
    std::swap(pipe_return_code_, rhs.pipe_return_code_);
    for (int i = 0; i < 4; i++) {
      std::swap(fd_[i], rhs.fd_[i]);
      std::swap(closed_[i], rhs.closed_[i]);
    }
    return *this;
  }
  bool is_ok() const { return pipe_return_code_ != -1; }
  void apply_redict_for_child() {
    dup2(fd_[child_read], STDIN_FILENO);
    dup2(fd_[child_write], STDOUT_FILENO);
  }
  int get_parent_read_fd() { return fd_[parent_read]; }
  int get_parent_write_fd() { return fd_[parent_write]; }
  int close_parent_read() { return close_pipe_by_index(parent_read); }
  int close_parent_write() { return close_pipe_by_index(parent_write); }
  int close_child_read() { return close_pipe_by_index(child_read); }
  int close_child_write() { return close_pipe_by_index(child_write); }

  void close_all() {
    for (int i = 0; i < 4; i++) {
      close_pipe_by_index(Pipe(i));
    }
  }

private:
  enum Pipe {
    parent_read,
    child_write,
    child_read,
    parent_write,
  };
  int close_pipe_by_index(Pipe index) {
    if (closed_[index]) {
      return 0;
    }
    auto result = close(fd_[index]);
    closed_[index] = true;
    return result;
  }

  int pipe_return_code_{};
  int fd_[4]{};
  bool closed_[4]{};
};
} // namespace subprocess