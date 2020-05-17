#include <fstream>
#include <iostream>

#include "gtest/gtest.h"
#include "Gzipper.hpp"

TEST(VerifyFileHeaders, CheckSampleFile) {
  std::ifstream file;
  file.open("../tests/testfiles/123.txt");

  file.seekg(0, std::ios_base::end);
  int len = file.tellg();

  file.seekg(0, std::ios_base::beg);

  std::cout << len << std::endl;

  std::cout << std::hex << Gzipper::GetCRCHash(file, len) << std::endl;
  file.close();
}

TEST(VerifyFileHeaders, CheckVerifyHeaders) {
  std::ifstream file;
  file.open("../tests/testfiles/compile_commands.json.gz");
  int dummy;
  ASSERT_GT(Gzipper::VerifyHeaders(file, dummy), 0);
}