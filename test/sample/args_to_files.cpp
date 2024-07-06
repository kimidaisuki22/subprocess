#include "../test_file_name.h"
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

int main(int argc, char **argv) {
  std::ofstream file{std::string{test_file_name}};
  if (!file) {
    return 1;
  }
  for (int i = 1; i < argc; i++) {
    file << argv[i] << "\n";
  }
}