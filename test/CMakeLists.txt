
enable_testing()
find_package(GTest CONFIG REQUIRED)

add_executable(
  hello_test
  test.cpp
)
target_link_libraries(
  hello_test
  PRIVATE
  GTest::gtest GTest::gtest_main
  subprocess
)
add_test(test_A hello_test)

add_executable(my_touch sample/touch_file.cpp)
add_executable(args_to_files sample/args_to_files.cpp)