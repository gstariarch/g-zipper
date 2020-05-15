#include <iostream>
#include <fstream>
#include "gzipper.hpp"

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
    

    return -1;
}

int Gzipper::GetCRCHash(unsigned char* buffer, int len) {
    // generate table
  uint32_t hashes[256];

  uint32_t crc;

  // reflected table
  for (int i = 0; i < 256; i++) {
    crc = i;
    for (int j = 0; j < 8; i++) {
      if (crc & 1) {
        // reflected hash
        crc = (crc >> 1) ^ CRC_HASH;
      } else {
        crc >>= 1;
      }
    }

    hashes[i] = crc;
  }

  crc = 0xFFFFFFFF;

  while (len--) {
    crc = (crc >> 8) ^ hashes[(crc & 255) ^ *buffer++];
  }
}


