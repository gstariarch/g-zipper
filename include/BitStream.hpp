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
   *  Reads bits in MSB order, interpreting the first bit read as the highest
   *  and reading sequentially.
   */ 
  uint64_t GetBitsMSB(int num_bits);

  /**
   *  Reads bits in LSB order, interpreting the first bit read as the lowest
   *  and reading sequentially.
   */ 
  uint64_t GetBitsLSB(int num_bits);
  uint8_t GetBit();

  /**
   *  For uncompressed data
   */ 
  void SkipToNextByte();
 private:
  uint8_t cache;
  uint8_t bit_cursor;
  std::istream* input;
};

#endif  // BITSTREAM_H_