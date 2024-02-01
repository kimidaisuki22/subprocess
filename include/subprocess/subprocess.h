#pragma once
#include "subprocess/process_controller.h"
#include <memory>
#include <subprocess/create_info.h>
#include <subprocess/link_def.h>
namespace subprocess {
SUBPROCESS_API std::unique_ptr<Controller> create(const Create_info_simplest &info);
} // namespace subprocess