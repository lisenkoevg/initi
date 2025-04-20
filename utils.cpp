#include <iomanip>
#include <iostream>
#include <vector>

using std::endl;
using std::cout;

void dumpUint(uint64_t val) {
  for (size_t i = 0; i < sizeof(val); ++i) {
    if (i != 0)
      cout << " ";
    std::byte *b = reinterpret_cast<std::byte *>(&val);
    cout << std::hex << std::setw(2) << std::setfill('0') << (0xff & static_cast<char>(*(b+i)));
  }
  cout << endl;
}

void dumpBuffer(const std::vector<std::byte> &b) {
  for (size_t i = 0; i != b.size(); ++i) {
    if (i != 0) cout << " ";
    cout << std::hex << std::setw(2) << std::setfill('0') << (0xff & static_cast<char>(b[i]));
  }
  cout << endl;
}

