#ifndef UTILS_H
#define UTILS_H

#include "types.h"

void dumpUint(uint64_t val);
void dumpBuffer(const std::vector<std::byte> &b);

Buffer serializeUint(uint64_t val);

// Read uint64_t start from given iterator and advance iterator forward by sizeof(uint64_t)
uint64_t deserializeUint(Buffer::const_iterator &);

void writeBufferToFile(const Buffer &b, const string &filename);
Buffer readBufFromFile(string filename);

#endif
