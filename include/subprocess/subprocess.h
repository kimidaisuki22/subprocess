#pragma once
#include "subprocess/process_controller.h"
#include <memory>
#include <subprocess/create_info.h>
namespace subprocess {
__declspec( dllexport ) std::unique_ptr<Controller> create(const Create_info_simplest &info);
} // namespace subprocess