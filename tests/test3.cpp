#include <iostream>
#include "../Serializator.h"

int main() {
  VectorType v;

  StringType s("qwerty");
  v.push_back(s);

  IntegerType i(100500);
  v.push_back(i);

  Serializator se;
  se.push(v);
  Buffer b;
  b = se.serialize();

  cout << se.toString() << endl;
  dumpBuffer(b);

  //   writeBufferToFile(b, "test3.bin");

  return 0;
}
