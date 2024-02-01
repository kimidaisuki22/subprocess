#include "../test_file_name.h"
#include <fstream>
#include <string_view>

int main(int argc, char **argv) {
  std::string filename =
      std::string{argc > 1 ? std::string_view{argv[1]} : test_file_name};
  std::ofstream file{filename};

  return file.is_open();
}