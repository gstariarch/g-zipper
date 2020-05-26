#include "LookbackOutputStream.hpp"

LookbackOutputStream::LookbackOutputStream() : LookbackOutputStream(16) { }

LookbackOutputStream::LookbackOutputStream(int initial_size) : buffer_capacity_(initial_size) {
  start_ = new char[initial_size];
  end_ = start_;
}

void LookbackOutputStream::PutLiteral(char input) {
  *end_++ = input;
  if (end_ - start_ >= buffer_capacity_) {
    ExpandBuffer();
  }
}

void LookbackOutputStream::Lookback(uint16_t length, uint16_t distance) {
  char* lookback_cursor = end_ - distance;
  if (lookback_cursor >= start_) {
    // lookback is valid
    int buffer_size = (end_ - start_);
    if (buffer_size + length >= buffer_capacity_) {
      ExpandBuffer();
    }

    while (length--) {
      // we do not need to resize
      *end_++ = *lookback_cursor++;
    }
  }
}

// by default just double the size
void LookbackOutputStream::ExpandBuffer() {
  char* new_buffer = new char[buffer_capacity_ * 2];
  int index = 0;
  while (index < buffer_capacity_) {
    new_buffer[index++] = *start_++;
  }

  end_ = new_buffer + (end_ - start_);
  delete[] start_;
  start_ = new_buffer;
  buffer_capacity_ *= 2;
}

std::string LookbackOutputStream::GetString() {
  *end_ = '\0';
  // more sensible explanation: std::move casts to rvalue
  // i dont know if this is necessary but it seems better
  return std::move(std::string(start_));
}

LookbackOutputStream::~LookbackOutputStream() {
  delete[] start_;
}