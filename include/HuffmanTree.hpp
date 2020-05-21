#ifndef HUFFMANTREE_H_
#define HUFFMANTREE_H_
#include <cinttypes>

/**
 *  Represents a single huffman node
 */ 
template <typename S>
struct huffman_node {
  S payload;
  bool is_leaf;
  huffman_node<S>* zero;
  huffman_node<S>* one;

  huffman_node() {
    zero = nullptr;
    one = nullptr;
    is_leaf = false;
  }
};

/**
 *  Class for a huffman tree
 * 
 *  If performance were priority we'd probably do it different but whatever
 */ 
template <typename T>
class HuffmanTree {
 public:

  // alternative constructor which assumes data elements start from 0 and are sequential,
  // allowing us to drop the payload argument
  HuffmanTree(const uint8_t* bit_lengths, int len) {
    uint8_t bl_count[33] = {0};
    int bl_max = 0;
    for (int i = 0; i < len; i++) {
      bl_count[bit_lengths[i]]++;
      if (bl_max < bit_lengths[i]) {
        bl_max = bit_lengths[i];
      }
    }

    uint32_t code = 0;
    uint32_t* next_code = new uint32_t[bl_max + 1];
    bl_count[0] = 0;
    for (int i = 1; i <= bl_max; i++) {
      code = (bl_count[i - 1] + code) << 1;
      next_code[i] = code;
    }

    root_ = new huffman_node<T>();
    huffman_node<T>* cur;
    huffman_node<T>* last;

    uint8_t length;
    code = 0;
    for (int i = 0; i < len; i++) {
      // account for zeroes on bit length
      if (bit_lengths[i] <= 0) {
        continue;
      }

      length = bit_lengths[i];
      code = next_code[length]++;
      code = ReverseBits32(code);
      // flip the code so we can shift it down
      code >>= (32 - length);

      // traverse from root
      cur = root_;
      last = root_;
      for (int j = 0; j < length; j++) {
        if (code & 1) {
          cur = last->one;
          if (cur == nullptr) {
            last->one = new huffman_node<T>();
            cur = last->one;
          }
        } else {
          cur = last->zero;
          if (cur == nullptr) {
            last->zero = new huffman_node<T>();
            cur = last->zero;
          }
        }

        last = cur;
        code >>= 1;
      }

      cur->is_leaf = true;
      cur->payload = i;
    }

    current_node_ = root_;

    delete[] next_code;
  }

  /**
   *  Take a step along the huffman tree.
   *  If we're at a leaf, return 0, set the output parameter, and jump back to the root.
   *  If we're at a non-leaf, return NON_LEAF.
   *  If we take an invalid step, return INVALID_NODE.
   */ 
  int Step(uint8_t bit, T* output) {
    if (bit & 1) {
      current_node_ = current_node_->one;
    } else {
      current_node_ = current_node_->zero;
    }

    if (current_node_ == nullptr) {
      current_node_ = root_;
      return INVALID_NODE;
    } else if (current_node_->is_leaf) {
      *output = current_node_->payload;
      current_node_ = root_;
      return 0;
    } else {
      return NON_LEAF;
    }
  }

  /**
   *  Resets the tree back to square one.
   */ 
  void Reset() {
    current_node_ = root_;
  }

  ~HuffmanTree() {

  }

  static const int NON_LEAF = 1;       // node is not a leaf
  static const int INVALID_NODE = 2;   // node is completely invalid
  

 private:
  huffman_node<T>* root_;
  huffman_node<T>* current_node_;

  uint32_t ReverseBits32(uint32_t input) {
    input = (input >> 16) | (input << 16);
    input = ((input >> 8) & 0x00FF00FF) | ((input << 8) & 0xFF00FF00);
    input = ((input >> 4) & 0x0F0F0F0F) | ((input << 4) & 0xF0F0F0F0);
    input = ((input >> 2) & 0x33333333) | ((input << 2) & 0xCCCCCCCC);
    input = ((input >> 1) & 0x55555555) | ((input << 1) & 0xAAAAAAAA);

    return input;
  }

  void FreeChildren(huffman_node<T>* node) {
    if (node != nullptr) {
      FreeChildren(node->zero);
      FreeChildren(node->one);
      // todo: if necessary to store anything more complex than an int,
      //       allow user to specify a payload-freeing callback
    }
  }
};

#endif  // HUFFMANTREE_H_