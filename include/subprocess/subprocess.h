#pragma once
#include "subprocess/process_controller.h"
#include <memory>
#include <subprocess/create_info.h>
namespace subprocess {
std::unique_ptr<controller> create(const Create_info_simplest &info);
} // namespace subprocess