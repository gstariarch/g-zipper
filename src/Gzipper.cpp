#include <iostream>
#include <fstream>
#include <limits>

#include "Gzipper.hpp"
#include "HuffmanTree.hpp"

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
    crc_type = stream.GetBitsLSB(2);

    // TODO: wait for implementation of these

    // switch (crc_type) {
    //   case 0x00:
    //     // uncompressed data
    //     HandleUncompressedData(&stream, output);
    //     break;
    //   case 0x01:
    //     // static compressed
    //     HandleStaticHuffmanData(&stream, output);
    //     break;
    //   case 0x10:
    //     // dynamic compressed
    //     HandleDynamicHuffmanData(&stream, output);
    //     break;
    //   default:
    //     // error case
    //     return -1;    
    // }
  }

  return 0;
}

uint32_t Gzipper::HandleUncompressedData(BitStream* stream, std::string& output) {
  return 1;
}

uint32_t Gzipper::HandleStaticHuffmanData(BitStream* stream, std::string& output) {
  return 1;
}

uint32_t Gzipper::HandleDynamicHuffmanData(BitStream* stream, std::string& output) {
  uint16_t literal_count = stream->GetBitsLSB(5) + 257;
  uint16_t distance_count = stream->GetBitsLSB(5) + 1;
  uint16_t codelen_count = stream->GetBitsLSB(4) + 4;

  // build huffman tree for codelens
  // stack vs heap

  // length of huffman encoding for each code length
  uint8_t code_lens[CODE_LENGTH_COUNT] = {0};
  // number of lengths with a given bit count

  // read code counts (MSB)
  for (int i = 0; i < codelen_count; i++) {
    code_lens[CODE_LENGTH_ORDER[i]] = stream->GetBitsMSB(3);
  }


  HuffmanTree<uint8_t> length_tree(code_lens, CODE_LENGTH_COUNT);
  // create literals table
  int literal = 0;
  uint8_t bit_length;
  uint8_t* literal_lens = new uint8_t[literal_count];

  uint8_t repeat_count;

  while (literal < literal_count) {
    // reach a leaf node
    while (length_tree.Step(stream->GetBit(), &bit_length) != 0);
    switch (bit_length) {
      case 16:
        // repeat last length some number of times (2 bits + 3)
        repeat_count = 3 + stream->GetBitsMSB(2);
        while (repeat_count--) {
          literal_lens[repeat_count + literal] = literal_lens[literal - 1];
        }

        break;
      case 17:
        // record some number of zeroes (3 bits + 3)
        repeat_count = 3 + stream->GetBitsMSB(3);
        while (repeat_count--) {
          literal_lens[literal++] = 0;
        }

        break;
      case 18:
        // record even more zeroes (7 bits + 11)
        repeat_count = 11 + stream->GetBitsMSB(7);
        while (repeat_count--) {
          literal_lens[literal++] = 0;
        }
        break;
      default:
        // just interpret the number
        literal_lens[literal++] = bit_length;
    }
  }

  HuffmanTree<uint16_t> literal_tree(literal_lens, literal_count);
  delete[] literal_lens;

  // parse distance tree
  uint8_t* distance_lens = new uint8_t[literal_count];
  literal = 0;
  length_tree.Reset();

  while (literal < distance_count) {
    while (length_tree.Step(stream->GetBit(), &bit_length) != 0);
    switch (bit_length) {
      case 16:
        repeat_count = 3 + stream->GetBitsMSB(2);
        while (repeat_count--) {
          distance_lens[literal++] = distance_lens[literal - 1];
        }
        break;
      case 17:
        repeat_count = 3 + stream->GetBitsMSB(3);
        while (repeat_count--) {
          distance_lens[literal++] = 0;
        }
        break;
      case 18:
        repeat_count = 11 + stream->GetBitsMSB(7);
        while (repeat_count--) {
          distance_lens[literal++] = 0;
        }
        break;
      default:
        distance_lens[literal++] = bit_length;
    }
  }

  HuffmanTree<uint8_t> distance_tree(distance_lens, distance_count);
  delete[] distance_lens;

  // read the huffman tree
  // step along the huffman tree until we get to the bottom
  // if it's a literal: append it
  // if not: calculate the extra bit count
  // read the extra bits for length (not compressed)
  // read the distance bits (compressed in distance_tree)
  // read the extra bits and add to a static offset
  // print back out
  return 1;
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




