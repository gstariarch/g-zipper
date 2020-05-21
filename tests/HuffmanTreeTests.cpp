#include <cinttypes>

#include "gtest/gtest.h"
#include "HuffmanTree.hpp"

static const uint8_t code_lengths[6] = {2, 2, 3, 0, 4, 3};
static const uint8_t LENGTH = 24; // valid bits in code
static const unsigned char CHAR_OFFSET = 0x41;  // 'A' in ascii
static const char result[10] = "CACABAEFC";  // result from default constructed huffman tree

TEST(HuffmanTreeTests, VerifyTreeConstruction) {
  HuffmanTree<uint8_t> test_tree(code_lengths, 6);

  uint32_t CODE_BITS = 0x0034C821;  // CACABADEC
  uint8_t cursor = 0;

  test_tree.Reset();
  unsigned char content;
  for (int i = 0; i < LENGTH; i++) {
    if (test_tree.Step((CODE_BITS & 1), &content) == 0) {
      ASSERT_EQ(content + CHAR_OFFSET, result[cursor++]);
      std::cout << (char)(content + CHAR_OFFSET);
    }

    CODE_BITS >>= 1;
  }

  std::cout << std::endl;
}