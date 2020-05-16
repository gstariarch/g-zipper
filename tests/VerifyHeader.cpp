#include <fstream>
#include <iostream>

#include "gtest/gtest.h"
#include "Gzipper.hpp"

TEST(CRCHashTest, CheckSampleFile) {
  std::ifstream file;
  file.open("../tests/testfiles/123.txt");

  std::cout << std::hex << Gzipper::GetCRCHash(file, 23) << std::endl;
}