#include "Gzipper.hpp"
#include <gtest/gtest.h>
#include <fstream>

#include <iostream>

TEST(GzipperTests, TestStaticHuffman) {
  std::string result;
  std::ifstream file;
  std::ifstream check;
  // compression on this file is static
  file.open("123.txt.gz");
  check.open("123.txt");
  Gzipper::Decompress(file, result);

  std::string checkstr;

  std::getline(check, checkstr);

  ASSERT_EQ(result, checkstr);

  std::cout << result << std::endl;

  file.close();
  check.close();
}

TEST(GzipperTests, TestDynamicHuffman) {
  std::string result;
  std::ifstream file;
  std::ifstream check;
  // compression on this file is static
  file.open("moby_dick.txt.gz");
  check.open("moby_dick.txt");
  Gzipper::Decompress(file, result);

  std::string checkstr((std::istreambuf_iterator<char>(check)), std::istreambuf_iterator<char>());

  ASSERT_EQ(result, checkstr);
  file.close();
  check.close();
}

TEST(GzipperTests, TheFullDick) {
  std::string result;
  std::ifstream file;
  std::ifstream check;
  // compression on this file is static
  file.open("the_dick.txt.gz");
  check.open("the_dick.txt");
  Gzipper::Decompress(file, result);

  std::string checkstr((std::istreambuf_iterator<char>(check)), std::istreambuf_iterator<char>());

  ASSERT_EQ(result, checkstr);
  file.close();
  check.close();
}