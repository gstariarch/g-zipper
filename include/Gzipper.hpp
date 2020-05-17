#ifndef GZIPPER_H_
#define GZIPPER_H_

#include <iostream>
#include <cinttypes>

#include "BitStream.hpp"

// decompress

namespace Gzipper {

// STRUCTS ~~

/**
 *  Extract info from gzip header
 */ 
struct gzip_header {
  unsigned short id;
  unsigned char compression_method;
  unsigned char flags;
  uint32_t mtime;
  unsigned char xfl;
  unsigned char os;
};

/**
 *  Single node on our huffman tree.
 */ 
struct huffman_node {
  uint16_t payload;
  huffman_node* zero;
  huffman_node* one;
};

std::string Decompress(std::ifstream& file_stream);

/**
 *  Returns the CRC32 hash of the inputted buffer.
 */ 
uint32_t GetCRCHash(std::ifstream& file, int len);

/**
 *  Verifies that the file's headers are valid.
 *  Once everything is verified, returns the offset at which
 *  we can start reading.
 * 
 *  Returns a -2 if the file has not been specified.
 *  Returns a -1 if the file can be read, but is invalid.
 */ 
int VerifyHeaders(std::ifstream& file_stream, int& byte_length);

/**
 *  Builds the huffman tree contained in the block
 */ 
static huffman_node* GetHuffmanTree(BitStream& bitstream);

static const unsigned short ID_VERIFY = 0x8B1F;


// GZIP FLAGS
static const unsigned char FLAG_TEXT = 1;
static const unsigned char FLAG_HCRC = 2;
static const unsigned char FLAG_EXTRA = 4;
static const unsigned char FLAG_NAME = 8;
static const unsigned char FLAG_COMMENT = 16;

static const unsigned int CRC_HASH = 0xEDB88320;

};

#endif  // GZIPPER_H_