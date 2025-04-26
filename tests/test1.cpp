#include <iostream>
#include "../types.h"
#include "../Serializator.h"
#include "../utils.h"

int main() {
  Serializator se;
  Buffer b = se.serialize();

  cout << se.toString() << endl;
  dumpBuffer(b);

  return 0;
}
