#include <iostream>
#include "../Serializator.h"

int main() {
  VectorType v;

  StringType s("qwerty");
  v.push_back(s);

  IntegerType i(100500);
  v.push_back(i);

  FloatType f(2);
  v.push_back(f);

  VectorType v2{v};
  v.push_back(v);

  Serializator se;
  se.push(v);
  Buffer b = se.serialize();

  cout << se.toString() << endl;
  dumpBuffer(b);

  return 0;
}
