#pragma once
#include <cstdint>
namespace subprocess {

class Controller {
public:
  virtual bool has_start() const = 0;
  virtual bool is_running() const = 0;
  virtual bool has_stopped() const = 0;

  virtual void stop() = 0;
  virtual void wait() = 0;

  virtual int return_code() const = 0;
  virtual uint64_t pid() const = 0;
};
} // namespace subprocess