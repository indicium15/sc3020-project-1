#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <cstdint>

typedef unsigned char uchar;

// Defines an address of a record stored as a block address with an offset.
struct Address
{
  void *blockAddress;
  int offset;

//Constructor
  Address(void* blockAddress, int offset) : blockAddress(blockAddress), offset(offset) {}
};

#endif 