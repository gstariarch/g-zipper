#ifndef BITSTREAM_H_
#define BITSTREAM_H_

#include <fstream>

/**
 *  Abstraction which deals with bits instead of bytes. Wraps around a file I/O to deliver bits vs bytes
 */ 
class BitStream {
 public:
  /**
   *  Create a bitstream from file input
   */ 
  BitStream(std::istream* input);
  /**
   *  Reads bits in consecutive order.
   */ 
  uint64_t GetBits(int num_bits);
  uint64_t GetBitsInverted(int num_bits);
  uint8_t GetBit();
 private:
  uint8_t cache;
  uint8_t bit_cursor;
  std::istream* input;
};

#endif  // BITSTREAM_H_