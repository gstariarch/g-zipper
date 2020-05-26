#include <gtest/gtest.h>
#include "LookbackOutputStream.hpp"

TEST(LOSTests, CreateLOSDefault) {
  LookbackOutputStream los;
  los.PutLiteral('c');
  std::string the = los.GetString();
  ASSERT_EQ(the, "c");
}