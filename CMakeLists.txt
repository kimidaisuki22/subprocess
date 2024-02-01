cmake_minimum_required(VERSION 3.23.0)
if(BUILD_TESTING)
  list(APPEND VCPKG_MANIFEST_FEATURES "test")
endif()
project(subprocess VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

add_library(subprocess src/subprocess.cpp)


target_include_directories(subprocess PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
