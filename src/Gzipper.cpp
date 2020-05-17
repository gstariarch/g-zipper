#include <iostream>
#include <fstream>
#include <limits>

#include "Gzipper.hpp"

int Gzipper::Decompress(std::ifstream& file_stream, std::string& output) {
  
  int offset = VerifyHeaders(file_stream);
  BitStream stream(&file_stream);

  if (offset < 0) {
    // error case
    return -1;
  }

  // begin reading
  uint8_t crc_final = 0;
  uint8_t crc_type = 0;
  while (!crc_final) {
    crc_final = stream.GetBit();
    crc_type = stream.GetBitsInverted(2);

    switch (crc_type) {
      case 0x00:
        // uncompressed data
        HandleUncompressedData(&stream, output);
        break;
      case 0x01:
        // static compressed
        HandleStaticHuffmanData(&stream, output);
        break;
      case 0x10:
        // dynamic compressed
        HandleDynamicHuffmanData(&stream, output);
        break;
      default:
        // error case
        return -1;    
    }
  }

  return 0;
}

int Gzipper::VerifyHeaders(std::ifstream& file_stream) {
    if (!file_stream.is_open()) {
        return -2;
    }

    file_stream.seekg(0, std::ios_base::beg);

    int offset;
    gzip_header header;

    file_stream.read((char*)&header, sizeof(gzip_header));

    if (header.id != ID_VERIFY) {
        std::cout << "ERROR: expected " << ID_VERIFY << ", got "
                  << header.id << std::endl; 
        return -1;
    }

    if (header.compression_method != 8) {
        std::cout << "invalid compression method of " << header.compression_method << std::endl;
        return -1;
    }

    // start dealing with flags
    if (header.flags & FLAG_EXTRA) {
      unsigned short extra_len;
      file_stream >> extra_len;
      file_stream.seekg(extra_len + 2, std::ios_base::cur);
      offset += (extra_len + 2);
    }

    if (header.flags & FLAG_NAME) {
      file_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\0');
    }

    if (header.flags & FLAG_COMMENT) {
      file_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\0');
    }

    if (header.flags & FLAG_HCRC) {
      // read from the start of the header to here
      int offset = file_stream.tellg();
      uint16_t crc_expected;
      file_stream >> crc_expected;

      file_stream.seekg(0, std::ios_base::beg);
      uint32_t crc = GetCRCHash(file_stream, offset);

      if (crc_expected != (crc & 0xFFFF)) {
        // invalid header
        std::cout << "Invalid header crc -- expected " << crc_expected
                  << ", got " << crc << std::endl;
        return -1;
      }

      std::cout << crc_expected << ", " << crc << std::endl;
    }

    return file_stream.tellg();
}

uint32_t Gzipper::GetCRCHash(std::ifstream& file, int len) {

  // https://tools.ietf.org/html/rfc1952
  // http://chrisballance.com/wp-content/uploads/2015/10/CRC-Primer.html

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
  char input;

  while (len--) {
    file.get(input);
    crc = (crc >> 8) ^ hashes[(crc & 255) ^ input];
  }

  return ~crc;
}

int32_t Gzipper::GetNodeLeaf(BitStream* stream, huffman_node* node) {
  while (node->payload < 0) {
    int bit = stream->GetBit();
    node = (bit ? node->one : node->zero);
    if (node == NULL) {
      return -1;
    }
  }

  return node->payload;
}




