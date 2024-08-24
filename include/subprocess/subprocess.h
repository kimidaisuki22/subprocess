#pragma once
#include "subprocess/process_controller.h"
#include "subprocess/process_controller_with_io.h"
#include <memory>
#include <subprocess/create_info.h>
#include <subprocess/link_def.h>
namespace subprocess {
SUBPROCESS_API std::unique_ptr<Controller>
create(const Create_info_simplest &info);
SUBPROCESS_API std::unique_ptr<Controller>
create(const Create_info_extend &info);
SUBPROCESS_API std::unique_ptr<Controller_with_IO>
create_with_io(const Create_info_extend &info);
} // namespace subprocess