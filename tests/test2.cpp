#include <iostream>
#include "../Serializator.cpp"

int main() {
  Any *a;

  VectorType v;

  StringType s("qwerty");
  v.push_back(static_cast<Any &>(s));

  IntegerType i(100500);
  v.push_back(static_cast<Any &>(i));

  Serializator se;
  se.push(v);
  Buffer b;
  b = se.serialize();
  dumpBuffer(b);
  cout << endl;
  writeBufferToFile(b, "test2.bin");

  return 0;
}
