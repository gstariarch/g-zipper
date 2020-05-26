#include "LookbackOutputStream.hpp"
#include "gtest/gtest.h"

TEST(LOSTests, CreateLOSDefault) {
  LookbackOutputStream los;
  los.PutLiteral('c');
  std::string the = los.GetString();
  ASSERT_EQ(the, "c");
}

TEST(LOSTests, AddMultipleLiterals) {
  LookbackOutputStream los;
  los.PutLiteral('c');
  los.PutLiteral('a');
  los.Lookback(2, 2);
  ASSERT_EQ(los.GetString(), "caca");
}

TEST(LOSTests, TestBufferExpand) {
  LookbackOutputStream los;
  const std::string test = "world of the monkeys";
  for (char c : test) {
    los.PutLiteral(c);
  }

  ASSERT_EQ(los.GetString(), test);
}