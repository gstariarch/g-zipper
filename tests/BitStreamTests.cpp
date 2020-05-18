#include <fstream>
#include <iostream>
#include <cinttypes>
#include <bitset>

#include "gtest/gtest.h"
#include "BitStream.hpp"

TEST(VerifyBitStream, VerifyBitsInv) {
  std::ifstream file_check;
  std::ifstream file_input;

  // both should read bits starting from lsb
  // read_bits should leave them in this order (reversed)
  // read bits inverse should flip them (aka: over 8 bits, it would just be the char)

  file_check.open("../tests/testfiles/123.txt");
  file_input.open("../tests/testfiles/123.txt");

  BitStream stream(&file_input);

  uint8_t check;
  uint8_t stream_bits;

  // verify inverted bits
  file_check.read((char*)&check, sizeof(uint8_t));
  stream_bits = stream.GetBitsLSB(8);

  std::cout << stream_bits << std::endl;
  std::cout << check << std::endl;

  ASSERT_EQ(check, stream_bits);

  file_check.read((char*)&check, sizeof(uint8_t));
  // should be flipped -- lsb order
  stream_bits = stream.GetBitsMSB(8);

  std::cout << std::bitset<8>(check) << std::endl;

  check = ((check >> 4) & 0x0F) | ((check << 4) & 0xF0);
  check = ((check >> 2) & 0x33) | ((check << 2) & 0xCC);
  check = ((check >> 1) & 0x55) | ((check << 1) & 0xAA);

  std::cout << std::bitset<8>(check) << std::endl;

  ASSERT_EQ(stream_bits, check);

  // verify partial shifts

  // bits are read MSB first
  // bytes are read LSB first
  // if valid:
  //    - read 7 and 9
  //    - (7 bit) | (9 bit << 7) = num

  uint32_t seven = stream.GetBitsLSB(7);
  uint32_t nine = stream.GetBitsLSB(9);

  uint32_t final_val = (seven) | (nine << 7);
  uint16_t check_val;
  file_check.read((char*)&check_val, sizeof(uint16_t));

  ASSERT_EQ(final_val, check_val);
}