#pragma once

#include <string>
#include <vector>
namespace subprocess {
struct Create_info_simplest {
  std::string execute_name;
};
struct Create_info_extend {
  std::string execute_name;
  std::vector<std::string> args;
};
} // namespace subprocess