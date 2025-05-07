#include <iostream>
#include "../Serializator.h"

int main() {
  auto b = readBufferFromFile("test3.bin");
  auto v = Serializator::deserialize(b);
  Serializator s;
  for (auto &i: v)
    s.push(i);
  cout << s.toString() << endl;
  dumpBuffer(b);
  return 0;
}
