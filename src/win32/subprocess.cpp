#include "subprocess/create_info.h"
#include "subprocess/process_controller.h"
#include <cstdint>
#include <subprocess/subprocess.h>

#include <windows.h>

namespace subprocess {
class WindowsController : public Controller {
private:
  HANDLE processHandle;
  DWORD processId;

public:
  WindowsController(HANDLE processHandle, DWORD processId)
      : processHandle(processHandle), processId(processId) {}

  ~WindowsController() {
    if (processHandle) {
      CloseHandle(processHandle);
    }
  }

  bool has_start() const override {
    // Implement based on processHandle validity
    return processHandle != nullptr;
  }

  bool is_running() const override {
    if (processHandle == NULL) {
      return false;
    }
    DWORD status;
    if (!GetExitCodeProcess(processHandle, &status)) {
      // Handle error if needed
      return false;
    }
    return status == STILL_ACTIVE;
  }

  bool has_stopped() const override {
    if (processHandle == NULL) {
      return true; // No process handle means it's not running
    }

    DWORD exitCode;
    if (!GetExitCodeProcess(processHandle, &exitCode)) {
      // If GetExitCodeProcess fails, assume the process has stopped.
      // Alternatively, you might want to handle this differently based on your
      // needs.
      return true;
    }

    // The process has stopped if the exit code is not STILL_ACTIVE.
    return exitCode != STILL_ACTIVE;
  }

  void stop() override {
    if (processHandle) {
      TerminateProcess(processHandle, 1);
    }
  }

  void wait() override {
    if (processHandle) {
      WaitForSingleObject(processHandle, INFINITE);
    }
  }

  int return_code() const override {
    DWORD exitCode = 0;
    if (processHandle && GetExitCodeProcess(processHandle, &exitCode)) {
      return static_cast<int>(exitCode);
    }
    return -1; // or some error code
  }
  uint64_t pid() const override { return processId; }
};

__declspec( dllexport ) std::unique_ptr<Controller> create(const Create_info_simplest &info) {
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // Create a subprocess.
  if (!CreateProcessA(
          NULL, // No module name (use command line)
          const_cast<char *>(info.execute_name.c_str()), // Command line
          NULL,  // Process handle not inheritable
          NULL,  // Thread handle not inheritable
          FALSE, // Set handle inheritance to FALSE
          0,     // No creation flags
          NULL,  // Use parent's environment block
          NULL,  // Use parent's starting directory
          &si,   // Pointer to STARTUPINFO structure
          &pi)   // Pointer to PROCESS_INFORMATION structure
  ) {
    // Handle error
  }

  // Close thread handle as it's not needed
  CloseHandle(pi.hThread);

  return std::make_unique<WindowsController>(pi.hProcess, pi.dwProcessId);
}

} // namespace subprocess
