#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>
#include <iomanip>
#include <type_traits>

using std::cout;
using std::endl;
using std::is_same;
using std::string;
using std::vector;

using Id = uint64_t;
using Buffer = vector<std::byte>;

enum class TypeId : Id { Uint, Float, String, Vector };

// clang-format off
string enumToStr(TypeId v) {
  switch (v) {
    case TypeId::Uint: return "Uint";
    case TypeId::String: return "String";
    case TypeId::Vector: return "Vector";
    default: std::cerr << "Wrong enum value" << endl; abort();
  }
}
// clang-format on

struct ValueStore {
  void dump() {
    cout << "type:" << enumToStr(valueType) << " ";
    size_t i = 0;
    for (auto &c : valueRaw) {
      cout << std::hex << (0xFF & static_cast<unsigned char>(c));
      ++i;
    }
  }
  TypeId valueType;
  Buffer valueRaw;
};

class IntegerType {
public:
  IntegerType(uint64_t val = 0) {
    vs.valueType = TypeId::Uint;
    vs.valueRaw = serialize(val);
    dump();
    cout << endl;
  }
  Buffer serialize(uint64_t val) const {
    auto ret = Buffer();
    std::copy(reinterpret_cast<std::byte *>(&val), reinterpret_cast<std::byte *>(&val) + sizeof(val),
              back_inserter(ret));
    return ret;
  }
  void dump() { vs.dump(); }

private:
  ValueStore vs;
};

struct StringType {
  StringType(const string &v = "") {}
};

struct Any {
public:
  template <typename T>
  Any(const T &&v) {
    //     payload = v.serialize();
    //     if (is_same<T, IntegerType>::value) {
    //       it = v;
    //     } else if (is_same<T, StringType>::value) {
    //       st = v;
    //     }
  }

  void serialize(Buffer &_buff) const {}

  template <typename Type>
  auto &getValue() const {
    const int &i = 2;
    return i;
  }

  template <TypeId kId>
  auto &getValue() const {
    const int &i = 1;
    return i;
  }

  Buffer payload;
};

class Serializator {
public:
  void push(Any v) { storage.push_back(v); }
  const vector<Any> &getStorage() const { return storage; }
  Buffer serialize() const {
    auto ret = Buffer();
    auto sz = storage.size();
    //     cout << sz << " " << sizeof(sz) << endl;
    //     cout << std::bitset<64>(sz) << endl;
    std::copy(reinterpret_cast<std::byte *>(&sz), reinterpret_cast<std::byte *>(&sz) + sizeof(sz), back_inserter(ret));
    return ret;
  }
  static vector<Any> deserialize(const Buffer &_val);

private:
  vector<Any> storage;
};

int main() {
  Serializator s;
  IntegerType i1;
  i1 = IntegerType(1 << 1);
  i1 = IntegerType(1 << 2);
  i1 = IntegerType(1 << 3);
  i1 = IntegerType(1 << 4);
  //   s.push(Any(i1));

  //   s.push(Any(IntegerType(3)));
  //   auto buf = s.serialize();
  //   std::ofstream("raw_test.bin", std::ios::out | std::ios::binary)
  //       .write(reinterpret_cast<const char *>(buf.data()), buf.size());
  //   cout << buf.size() << endl;
  //   size_t i = 0;
  //   for (auto &c : buf) {
  //     cout << static_cast<char>(c);
  //     cout << std::bitset<8>(static_cast<unsigned char>(c)) << " ";
  //     cout << std::hex << (0xFF & static_cast<unsigned char>(c));
  //     ++i;
  //     if (i % 8 == 0)
  //       cout << " ";
  //   }
  //   cout << endl;
  //   Any a;
  //   cout << a.getValue<TypeId::Uint>() << endl;
  //   cout << a.getValue<Any>() << endl;
}
