#include <iostream>
#include "../Serializator.h"

int main() {
  VectorType v;

  Serializator se;
  se.push(v);
  Buffer b = se.serialize();

  cout << se.toString() << endl;
  dumpBuffer(b);

  return 0;
}
