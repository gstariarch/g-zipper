#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>

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

  // TODO: separate this tree building into a helper function maybe?

  // build huffman tree for codelens
  // stack vs heap

  // length of huffman encoding for each code length
  uint8_t code_lens[CODE_LENGTH_COUNT] = {0};
  // number of lengths with a given bit count

  // read code counts (MSB)
  for (int i = 0; i < codelen_count; i++) {
    code_lens[CODE_LENGTH_ORDER[i]] = stream->GetBitsLSB(3);
  }


  HuffmanTree<uint8_t> length_tree(code_lens, CODE_LENGTH_COUNT);
  // create literals table
  int literal = 0;
  uint8_t tree_output;
  uint8_t* literal_lens = new uint8_t[literal_count];

  uint8_t repeat_count;
  uint8_t last_literal;

  while (literal < literal_count) {
    // reach a leaf node
    while (length_tree.Step(stream->GetBit(), &tree_output) != 0);
    
    
    switch (tree_output) {
      case 16:
        // repeat last length some number of times (2 bits + 3)
        last_literal = literal_lens[literal - 1];
        repeat_count = 3 + stream->GetBitsLSB(2);
        while (repeat_count--) {
          literal_lens[repeat_count + literal] = last_literal;
        }

        break;
      case 17:
        // record some number of zeroes (3 bits + 3)
        repeat_count = 3 + stream->GetBitsLSB(3);
        while (repeat_count--) {
          literal_lens[literal++] = 0;
        }

        break;
      case 18:
        // record even more zeroes (7 bits + 11)
        repeat_count = 11 + stream->GetBitsLSB(7);
        while (repeat_count--) {
          literal_lens[literal++] = 0;
        }
        break;
      default:
        // just interpret the number
        literal_lens[literal++] = tree_output;
    }
  }

  HuffmanTree<uint16_t> literal_tree(literal_lens, literal_count);
  delete[] literal_lens;

  // parse distance tree
  uint8_t* distance_lens = new uint8_t[literal_count];
  literal = 0;
  length_tree.Reset();

  while (literal < distance_count) {
    while (length_tree.Step(stream->GetBit(), &tree_output) != 0);
    switch (tree_output) {
      case 16:
        repeat_count = 3 + stream->GetBitsLSB(2);
        last_literal = distance_lens[literal - 1];
        while (repeat_count--) {
          distance_lens[literal++] = last_literal;
        }
        break;
      case 17:
        repeat_count = 3 + stream->GetBitsLSB(3);
        while (repeat_count--) {
          distance_lens[literal++] = 0;
        }
        break;
      case 18:
        repeat_count = 11 + stream->GetBitsLSB(7);
        while (repeat_count--) {
          distance_lens[literal++] = 0;
        }
        break;
      default:
        distance_lens[literal++] = tree_output;
    }
  }

  HuffmanTree<uint8_t> distance_tree(distance_lens, distance_count);
  delete[] distance_lens;

  // read the huffman tree
  literal = 0;
  literal_tree.Reset();
  distance_tree.Reset();

  uint16_t literal_output;

  do {
    while (literal_tree.Step(stream->GetBit(), &literal_output) != 0);
    if (literal_output < END_OF_BLOCK) {
      // literal
      output.push_back(static_cast<char>(literal_output));
    } else if (literal_output > END_OF_BLOCK) {
      // get length code
      uint16_t lookback_length;
      if (literal_output > 264) {
        uint8_t bit_count = (literal_output - 261) / 4;
        lookback_length = stream->GetBitsLSB(bit_count) + LENGTH_CONSTANTS[literal_output - 265];
      } else {
        lookback_length = 3 + (literal_output - 257);
      }

      uint8_t distance_output;
      uint16_t lookback_distance;
      while (distance_tree.Step(stream->GetBit(), &distance_output) != 0);

      if (distance_output < 4) {
        lookback_distance = distance_output + 1;
      } else {
        uint8_t bit_count = (distance_output - 2) / 2;
        lookback_distance = stream->GetBitsLSB(bit_count) + DIST_CONSTANTS[distance_output - 4];
      }

      // need to define
      // read back from previous
      // uh oh
      // TODO: create a wrapper for this?
      //    - allow for quick access to previous elements,
      //      while allowing quick character placement (only chars)
      //      could use a vector i guess
      

      // the ordering deal:
      // everything that isn't a huffman code (referring to a path down a given tree) is LSB first.
      // huffman codes start with the MSB, meaning the root of the tree. This is why we can go bit by bit as we slink down them
      // i dont know why i didnt realize that earlier lmao
    }
  } while (literal_output != END_OF_BLOCK);

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




