#ifndef LOOKBACK_OUTPUT_STREAM_H_
#define LOOKBACK_OUTPUT_STREAM_H_

#include <cinttypes>
#include <string>

/**
 *  A string wasn't great, a vector was a bit better,
 *  but since this is a fuck-around i thought it would be good
 *  to encapsulate the writing process in this stream
 *  which allows lookbacks and literals very easily
 */ 
class LookbackOutputStream {
 public:

  /**
   *  Creates a new stream with an arbitrary initial size.
   */ 
  LookbackOutputStream();

  /**
   *  Creates a new stream with the size dictated by the parameter value.
   */ 
  LookbackOutputStream(int initial_size);

  /**
   *  Place a literal in the stream.
   */ 
  void PutLiteral(char input);

  /**
   *  Look back (distance) characters from the end of the stream
   *  and read (length) characters to its front.
   */ 
  void Lookback(uint16_t length, uint16_t distance);

  // use one of these probably
  std::string GetString();

  ~LookbackOutputStream();
 private:

  /**
   *  if we run out of space, expand the buffer size.
   */ 
  void ExpandBuffer();
  char* start_;
  char* end_;
  int buffer_capacity_;
};

#endif