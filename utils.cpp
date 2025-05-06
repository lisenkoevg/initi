#include "utils.h"

void dumpUint(uint64_t val) {
  for (uint64_t i = 0; i < sizeof(val); ++i) {
    if (i != 0)
      cout << " ";
    std::byte *b = reinterpret_cast<std::byte *>(&val);
    cout << std::hex << std::setw(2) << std::setfill('0') << (0xff & static_cast<char>(*(b + i)));
  }
  cout << endl;
}

void dumpBuffer(const std::vector<std::byte> &b) {
  for (uint64_t i = 0; i != b.size(); ++i) {
    if (i != 0) {
      if (i % 8 != 0)
        cout << " ";
      else {
        cout << endl;
        if (i % 64 == 0)
          cout << endl;
      }
    }
    cout << std::hex << std::setw(2) << std::setfill('0') << (0xff & static_cast<char>(b[i]));
  }
  cout << endl;
}

Buffer serializeUint(uint64_t val) {
  std::byte *b = reinterpret_cast<std::byte *>(&val);
  uint64_t sz = sizeof(val);
  auto ret = Buffer();
  for (uint64_t i = 0; i != sz; ++i) {
    ret.push_back(*(b + i));
  }
  return ret;
}

uint64_t deserializeUint(Buffer::const_iterator &iter) {
  uint64_t ret = 0;
  for (uint64_t i = 0; i != sizeof(uint64_t); ++i, ++iter)
    *(reinterpret_cast<std::byte *>(&ret) + i) = *iter;
  return ret;
}

void writeBufferToFile(const Buffer &b, const string &filename) {
  std::ofstream f = std::ofstream(filename, std::ios::out | std::ios::binary);
  f.write(reinterpret_cast<const char *>(b.data()), b.size());
  f.close();
}

Buffer readBufferFromFile(string filename) {
  ifstream raw;
  std::streamsize size = 0;
  raw.open("tmp/raw_test.bin", std::ios::in | std::ios::binary);
  if (!raw.is_open())
    return Buffer(size);

  raw.seekg(0, std::ios::end);
  size = raw.tellg();
  raw.seekg(0, std::ios::beg);

  Buffer b(size);
  raw.read(reinterpret_cast<char*>(b.data()), size);
  raw.close();
  return b;
}
