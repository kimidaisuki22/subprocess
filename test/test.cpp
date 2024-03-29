#include "subprocess/create_info.h"
#include "test_file_name.h"
#include <filesystem>
#include <gtest/gtest.h>
#include <subprocess/subprocess.h>
// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect equality.
  EXPECT_FALSE(std::filesystem::exists(test_file_name));
  subprocess::Create_info_simplest create_info{};
  create_info.execute_name = "./my_touch";
  auto proc = subprocess::create(create_info);
  EXPECT_NE(proc, nullptr);
  proc->wait();
  EXPECT_TRUE(proc->has_stopped());
  EXPECT_EQ(proc->return_code(), 0);

  EXPECT_TRUE(std::filesystem::exists(test_file_name));
  std::filesystem::remove(test_file_name);
}
