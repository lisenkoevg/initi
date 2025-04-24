#include <iostream>
#include "../Serializator.cpp"

int main() {
  Serializator se;
  Buffer b = se.serialize();

  cout << se.toString() << endl;
  dumpBuffer(b);

  return 0;
}
