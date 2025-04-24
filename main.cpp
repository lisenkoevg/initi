#include <iostream>
#include "Serializator.cpp"

Buffer readBuf(string filename) {
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

int main() {

  Buffer b = readBuf("tmp/raw_test.bin");
  cout << "Buffer size: " << b.size() << endl;

  auto vec = Serializator::deserialize(b);
  cout << "Objects count: " << vec.size() << endl;
  cout << "Object 0 type: " << toString(vec[0].getTypeId()) << endl;
  return 0;
}
