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
      // error: when expanding the buffer, the lookback cursor is stuck at the old array instead of the new one
      lookback_cursor = end_ - distance;
    }

    while (length--) {
      // we do not need to resize here
      *end_++ = *lookback_cursor++;
    }
  }
}

// by default just double the size
void LookbackOutputStream::ExpandBuffer() {
  char* start_old = start_;
  start_ = new char[buffer_capacity_ * 2];
  int index = 0;
  while (index < buffer_capacity_) {
    start_[index] = start_old[index];
    index++;
  }

  end_ = start_ + (end_ - start_old);
  delete[] start_old;
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