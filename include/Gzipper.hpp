#ifndef GZIPPER_H_
#define GZIPPER_H_

#include <iostream>
#include <cinttypes>

#include "BitStream.hpp"
#include "LookbackOutputStream.hpp"

// decompress

namespace Gzipper {

// STRUCTS

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
 *  Decompress the inputted file.
 * 
 *  @param file_stream - the stream we are decompressing. Must already be opened.
 *  @param output - the string we are outputting to.
 * 
 *  @return - idk yet to be honest, we'll figure it out :)
 */ 
int Decompress(std::ifstream& file_stream, std::string& output);

/**
 *  Returns the CRC32 hash of the inputted buffer.
 * 
 *  @param file - the file we are hashing. Its position is not reset, so you decide where to start.
 *  @param len - the number of bytes to hash.
 * 
 *  @return - The 32-bit remainder of the hashing function.
 */ 
uint32_t GetCRCHash(std::ifstream& file_stream, int len);

/**
 *  Verifies that the file's headers are valid.
 *  Once everything is verified, returns the offset at which
 *  we can start reading.
 * 
 *  Returns a -2 if the file has not been specified.
 *  Returns a -1 if the file can be read, but is invalid.
 * 
 *  @param file_stream - the GZIP file we want to verify.
 *  @returns - if fails, see above. If success, the offset where the remainder of the file starts.
 */ 
int VerifyHeaders(std::ifstream& file_stream);

/**
 *  The following three functions adhere to the following parameters:
 *  @param stream - the bit stream we are reading from.
 *  @param output - the string we are outputting the result to.
 */ 

/**
 *  Handles Gzip blocks where the data is not compressed, outputting contents to the output string.
 */ 
void HandleUncompressedData(std::ifstream& file_stream, LookbackOutputStream* output);

/**
 *  Handles Gzip blocks where the data is statically compressed.
 */ 
void HandleStaticHuffmanData(BitStream* stream, LookbackOutputStream* output);

/**
 *  Handles Gzip blocks where the data is dynamically compressed.
 */ 
void HandleDynamicHuffmanData(BitStream* stream, LookbackOutputStream* output);

static const unsigned short ID_VERIFY = 0x8B1F;


// GZIP FLAGS
static const unsigned char FLAG_TEXT = 1;
static const unsigned char FLAG_HCRC = 2;
static const unsigned char FLAG_EXTRA = 4;
static const unsigned char FLAG_NAME = 8;
static const unsigned char FLAG_COMMENT = 16;

// polynomial for CRC function -- reversed for her pleasure
static const unsigned int CRC_HASH = 0xEDB88320;

// constants for hclen
static const uint8_t MAX_CODE_LENGTH = 7;
static const uint8_t CODE_LENGTH_COUNT = 19;
static const uint8_t CODE_LENGTH_ORDER[CODE_LENGTH_COUNT] = {16, 17, 18,  0,  8,
                                                              7,  9,  6, 10,  5, 
                                                             11,  4, 12,  3, 13, 
                                                                  2, 14,  1, 15};

// decoding constants for lookback length, starting from 264
static const uint16_t LENGTH_CONSTANTS[20] = {11,  13,  15,  17,  19, 
                                              23,  27,  31,  35,  43,
                                              51,  59,  67,  83,  99,
                                              115, 131, 163, 195, 227};

// decoding constants for lookback distance, starting from code 4
static const uint16_t DIST_CONSTANTS[26] = {5,     7,    9,     13,    17,
                                            25,    33,   49,    65,    97,
                                            129,   193,  257,   385,   513,
                                            769,   1025, 1537,  2049,  3073,
                                            4097,  6145, 8193,  12289, 16385,
                                            24577 };

// Symbol for "end of block"
static const uint16_t END_OF_BLOCK = 256;


// huffman codes are MSB first
// read 7 bits and see if in range
// then shift and read 8th bit into lsb and see fi in either range
// then shift last bit and check that one
// if none then throw some invalid
// encodings for static huffman blocks
static const uint16_t SEVEN_BIT_LOWER_BOUND = 0x00;
static const uint16_t SEVEN_BIT_UPPER_BOUND = 0x17;
static const uint16_t SEVEN_BIT_OFFSET = 256;

static const uint16_t LOWER_EIGHT_BIT_LOWER_BOUND = 0x30;
static const uint16_t LOWER_EIGHT_BIT_UPPER_BOUND = 0xBF;
static const uint16_t LOWER_EIGHT_BIT_OFFSET = 0;

static const uint16_t UPPER_EIGHT_BIT_LOWER_BOUND = 0xC0;
static const uint16_t UPPER_EIGHT_BIT_UPPER_BOUND = 0xC7;
static const uint16_t UPPER_EIGHT_BIT_OFFSET = 280;

static const uint16_t NINE_BIT_LOWER_BOUND = 0x190;
static const uint16_t NINE_BIT_UPPER_BOUND = 0x1FF;
static const uint16_t NINE_BIT_OFFSET = 144;
};

#endif  // GZIPPER_H_