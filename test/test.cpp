#include "subprocess/create_info.h"
#include "test_file_name.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <subprocess/subprocess.h>
#include <vector>
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

TEST(HelloTest, Extend_no_space_one_file) {
  std::string filename = "my_file";
  EXPECT_FALSE(std::filesystem::exists(filename));
  subprocess::Create_info_extend create_info{};
  create_info.execute_name = "./my_touch";
  create_info.args = {filename};
  auto proc = subprocess::create(create_info);
  EXPECT_NE(proc, nullptr);
  proc->wait();
  EXPECT_TRUE(proc->has_stopped());
  EXPECT_EQ(proc->return_code(), 0);

  EXPECT_TRUE(std::filesystem::exists(filename));
  std::filesystem::remove(filename);
}

TEST(HelloTest, Extend_space_one_file) {
  std::string filename = "my file";
  EXPECT_FALSE(std::filesystem::exists(filename));
  subprocess::Create_info_extend create_info{};
  create_info.execute_name = "./my_touch";
  create_info.args = {filename};
  auto proc = subprocess::create(create_info);
  EXPECT_NE(proc, nullptr);
  proc->wait();
  EXPECT_TRUE(proc->has_stopped());
  EXPECT_EQ(proc->return_code(), 0);

  EXPECT_TRUE(std::filesystem::exists(filename));
  std::filesystem::remove(filename);
}

TEST(HelloTest, Extend_space_three_args) {
  std::vector<std::string> texts{"my file", "line_2", "Last'Time", "End; Quota\" "};
  EXPECT_FALSE(std::filesystem::exists(test_file_name));
  subprocess::Create_info_extend create_info{};
  create_info.execute_name = "./args_to_files";
  create_info.args = texts;
  auto proc = subprocess::create(create_info);
  EXPECT_NE(proc, nullptr);
  proc->wait();
  EXPECT_TRUE(proc->has_stopped());
  EXPECT_EQ(proc->return_code(), 0);
  {
    std::ifstream file{std::string{test_file_name}};
    std::vector<std::string> output;
    while (file) {
      std::string line;
      std::getline(file, line);
      output.push_back(line);
    }
    // drop empty line.
    if (!output.empty()) {
      output.pop_back();
    }
    EXPECT_EQ(output.size(), texts.size());
    for (int i = 0; i < output.size(); i++) {
      EXPECT_EQ(texts[i], output[i]);
    }
  }

  EXPECT_TRUE(std::filesystem::exists(test_file_name));
  std::filesystem::remove(test_file_name);
}
