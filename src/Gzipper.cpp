#include <iostream>
#include <fstream>
#include <limits>
#include <bitset>
#include "Gzipper.hpp"

int Gzipper::VerifyHeaders(std::ifstream& file_stream, int& byte_length) {
    if (!file_stream.is_open()) {
        return -2;
    }

    file_stream.seekg(0, std::ios_base::beg);

    // verify bits
    unsigned short id;
    file_stream >> id;

    #if DEBUG
        std::cout << "ID read: " << id << std::endl;
    #endif

    if (id != ID_VERIFY) {
        std::cout << "ERROR: expected " << ID_VERIFY << ", got "
                  << id << std::endl; 
        return -1;
    }

    unsigned char compression_method;
    // all i can find is deflate so lets go w that
    file_stream >> compression_method;
    if (compression_method != 8) {
        std::cout << "invalid compression method of " << compression_method << std::endl;
    }

    // GZIP FLAGS
    unsigned char flags;
    file_stream >> flags;

    // CURRENT TIME
    uint32_t mtime;
    file_stream >> mtime;

    // DEFLATE FLAGS
    unsigned char xfl;
    file_stream >> xfl;

    unsigned char os;
    file_stream >> os;

    // start dealing with flags
    if (flags & FLAG_EXTRA) {
      unsigned short extra_len;
      file_stream >> extra_len;
      file_stream.seekg(extra_len + 2, std::ios_base::cur);
    }

    if (flags & FLAG_NAME) {
      // skip to end of name string
      file_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\0');
    }

    if (flags & FLAG_COMMENT) {
      file_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\0');
    }

    if (flags & FLAG_HCRC) {
      // read from the start of the header to here
      int offset = file_stream.tellg();
      uint16_t crc_expected;
      file_stream >> crc_expected;

      file_stream.seekg(0, std::ios_base::beg);
      uint32_t crc = GetCRCHash(file_stream, offset);

      if (crc_expected != crc & 0xFFFF) {
        // invalid header
        std::cout << "Invalid header crc -- expected " << crc_expected
                  << ", got " << crc << std::endl;
        return -1;
      }
    }


    

    return -1;
}

uint32_t Gzipper::GetCRCHash(std::ifstream& file, int len) {
    // generate table
  uint32_t hashes[256];
  uint32_t crc;

  for (int i = 0; i < 256; i++) {
    crc = i;
    for (int j = 0; j < 8; j++) {
      if (crc & 1) {
        crc = (crc >> 1) ^ CRC_HASH;
      } else {
        crc >>= 1;
      }
    }

    hashes[i] = crc;
  }

  crc = 0xFFFFFFFF;
  uint32_t crc_test = 0xFFFFFFFF;
  char input;

  while (len--) {
    file.get(input);

    crc = (crc >> 8) ^ hashes[(crc & 255) ^ input];
  }

  return ~crc;
}


