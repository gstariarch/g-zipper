#include "BitStream.hpp"

BitStream::BitStream(std::istream* input) {
  this->input = input;
  cache = 0;
  bit_cursor = 8;
}

uint64_t BitStream::GetBitsLSB(int num_bits) {
  uint64_t result = 0;
  for (int i = 0; i < num_bits; i++) {
    result |= (GetBit() << i);
  }

  return result;
}

uint64_t BitStream::GetBitsMSB(int num_bits) {
  uint64_t result = 0;
  while (num_bits--) {

    result = (result << 1) | GetBit();
  }

  return result;
}

uint8_t BitStream::GetBit() {
  if (bit_cursor == 8) {
    bit_cursor -= 8;
    input->read((char*)&cache, sizeof(uint8_t));
    // handle EOF
  }

  uint8_t res = cache & 1;
  cache >>= 1;
  bit_cursor++;
  return res;
}

void BitStream::SkipToNextByte() {
  bit_cursor = 8;
  // dumps the cache on next read
}