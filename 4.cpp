/*
Попытка создать прототип, выполняющий то, что требуется по ТЗ,
но не обязательно соответствующий условиям ТЗ
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include "utils.cpp"

using std::cout;
using std::endl;
using std::string;
using std::vector;

using Id = uint64_t;
using Buffer = vector<std::byte>;

enum class TypeId : Id { Uint, Float, String, Vector };

Buffer serializeUint(uint64_t val) {
  std::byte *b = reinterpret_cast<std::byte *>(&val);
  size_t sz = sizeof(val);
  auto ret = Buffer();
  for (size_t i = 0; i != sz; ++i) {
    ret.push_back(*(b + i));
  }
  return ret;
}

class Any {
public:
  Any(TypeId tid) : typeId(tid) {}
  TypeId getTypeId() const { return typeId; }

protected:
  Buffer serialize(Buffer val) {
    auto ret = Buffer();
    auto bTypeId = serializeUint(static_cast<uint64_t>(typeId));
    std::copy(bTypeId.begin(), bTypeId.end(), back_inserter(ret));
    std::copy(val.begin(), val.end(), back_inserter(ret));
    return ret;
  }

private:
  TypeId typeId;
};

class IntegerType : public Any {
public:
  IntegerType(uint64_t val) : Any(TypeId::Uint), value(val) {}
  Buffer serialize() { return Any::serialize(serializeUint(value)); }

private:
  uint64_t value;
};

class StringType : public Any {
public:
  StringType(string val) : Any(TypeId::String), value(val) {}
  Buffer serialize() {
    Buffer ret = serializeUint(value.size());
    std::for_each(value.begin(), value.end(), [&ret](auto v) { ret.push_back(static_cast<std::byte>(v)); });
    return Any::serialize(ret);
  }

private:
  string value;
};

class VectorType : public Any {
public:
  VectorType() : Any(TypeId::Vector) {}

  Buffer serialize() {
    auto ret = serializeUint(items.size());
    auto retItems = VectorType::serialize(items);
    std::copy(retItems.begin(), retItems.end(), back_inserter(ret));
    return Any::serialize(ret);
  }

  void push_back(Any &val) { items.push_back(&val); }

  static Buffer serialize(vector<Any *> v) {
    auto ret = Buffer();
    for (auto cur = v.begin(); cur != v.end(); ++cur) {
      Any *item = *cur;
//       cout <<  static_cast<Id>(item->getTypeId()) << endl;
      Buffer b;
      if (item->getTypeId() == TypeId::Uint) {
        b = static_cast<IntegerType *>(item)->serialize();
      } else if (item->getTypeId() == TypeId::String) {
        b = static_cast<StringType *>(item)->serialize();
      } else if (item->getTypeId() == TypeId::Vector) {
        cout << "Vector" << endl;
      }
      std::copy(b.begin(), b.end(), back_inserter(ret));
    }
    return ret;
  }

private:
  vector<Any *> items;
};

class Serializator {
public:
  void push(Any a) { storage.push_back(&a); }
  Buffer serialize() {
    auto ret = serializeUint(storage.size());
    auto retItems = VectorType::serialize(storage);
    std::copy(retItems.begin(), retItems.end(), back_inserter(ret));
    return ret;
  }

private:
  vector<Any *> storage;
};

int main() {

  VectorType v;

  StringType s("qwerty");
  v.push_back(static_cast<Any &>(s));

  IntegerType i(100500);
  v.push_back(static_cast<Any &>(i));

  /*
  03 00 00 00 00 00 00 00
  02 00 00 00 00 00 00 00
  02 00 00 00 00 00 00 00
  06 00 00 00 00 00 00 00
  71 77 65 72 74 79 00 00
  00 00 00 00 00 00 94 88
  01 00 00 00 00 00
  */
  dumpBuffer(v.serialize());
  cout << endl;

  Serializator se;
  se.push(v);
  dumpBuffer(se.serialize());
  return 0;
}
