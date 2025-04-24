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

  Serializator se;
  se.push(v);

  cout << se.toString() << endl;

  return 0;
}
