#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include "utils.cpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

using Id = uint64_t;
using Buffer = vector<std::byte>;

enum class TypeId : Id { Uint, Float, String, Vector };

struct Any {
  Any(TypeId tid) : typeId(tid) {}
  TypeId typeId;
  Buffer raw;

  template <typename T>
  void append(T &&val, size_t sz = sizeof(val)) {
    cout << __PRETTY_FUNCTION__ << endl;
//     cout << val[0] << endl;
    std::byte *b = reinterpret_cast<std::byte *>(&val);
    for (size_t i = 0; i < sz; ++i) {
      cout << static_cast<char>(*(b + i)) << endl;
      raw.push_back(*(b + i));
    }
  }
};

struct IntegerType : public Any {
  IntegerType(uint64_t val) : Any(TypeId::Uint), value(val) { serialize(val); }

  void serialize(uint64_t val) {
    append(static_cast<Id>(TypeId::Uint));
    append(val);
  }
  uint64_t value;
};

struct StringType : public Any {
  StringType(string val) : Any(TypeId::String), value(val) { serialize(val); }

  void serialize(string &val) {
    append(static_cast<Id>(TypeId::String));
    append(val.size());
    append(val, val.size());
  }
  string value;
};

struct VectorType : public Any {
  VectorType() : Any(TypeId::Vector) {}
  vector<Any *> children;
};

int main() {
  StringType s(string("qwerty"));
  dumpBuffer(s.raw);

//      IntegerType i(100500);
//      dumpUint(i.value);
//      dumpBuffer(i.raw);
  //   std::ofstream("raw_test1.bin", std::ios::out | std::ios::binary)
  //       .write(reinterpret_cast<const char *>(i.raw.data()), i.raw.size());
//   VectorType v;
//   v.children.push_back(&s);
  //   v.children.push_back(&i);
}
