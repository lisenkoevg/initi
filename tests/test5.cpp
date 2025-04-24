#include <iostream>
#include "../Serializator.cpp"

int main() {
  VectorType v;

  StringType s("qwerty");
  v.push_back(static_cast<Any &>(s));

  IntegerType i(100500);
  v.push_back(static_cast<Any &>(i));

  FloatType f(2);
  v.push_back(static_cast<Any &>(f));

  VectorType v2{v};
  v.push_back(static_cast<Any &>(v2));

  Serializator se;
  se.push(v);
  Buffer b = se.serialize();

  cout << se.toString() << endl;
  dumpBuffer(b);

  return 0;
}
