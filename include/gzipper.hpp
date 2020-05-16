#ifndef GZIPPER_H_
#define GZIPPER_H_

#include <iostream>
#include <cinttypes>

// decompress
class Gzipper {
 public:
  static std::string Decompress(std::ifstream& file_stream);

  /**
   *  Returns the CRC32 hash of the inputted buffer.
   */ 
  static uint32_t GetCRCHash(std::ifstream& file, int len);
 private:
  /**
   *  Verifies that the file's headers are valid.
   *  Once everything is verified, returns the offset at which
   *  we can start reading.
   * 
   *  Returns a -2 if the file has not been specified.
   *  Returns a -1 if the file can be read, but is invalid.
   */ 
  static int VerifyHeaders(std::ifstream& file_stream, int& byte_length);

  static const short ID_VERIFY = 0x1f8b;

  // GZIP FLAGS
  static const unsigned char FLAG_TEXT = 1;
  static const unsigned char FLAG_HCRC = 2;
  static const unsigned char FLAG_EXTRA = 4;
  static const unsigned char FLAG_NAME = 8;
  static const unsigned char FLAG_COMMENT = 16;

  static const unsigned int CRC_HASH = 0xEDB88320;
};

#endif  // GZIPPER_H_