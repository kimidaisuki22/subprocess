#pragma once
#include "subprocess/process_controller.h"
#include <cstddef>
#include <cstdint>
namespace subprocess {
// sync IO.
class Controller_with_IO : virtual public Controller {
public:
  using Length = int64_t;
  virtual Length read(void *buffer, size_t read_size) = 0;
  virtual Length write(const void *buffer, size_t read_size) = 0;
  virtual bool close_read() = 0;
  virtual bool close_write() = 0;
};
} // namespace subprocess