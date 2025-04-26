#ifndef SERIALIZATOR_H
#define SERIALIZATOR_H

#include "types.h"
#include "utils.h"

#define PR (cout << __PRETTY_FUNCTION__ << endl);

string toString(TypeId);

// clang-format off
string toString(TypeId v) {
  string ret = "TypeId::";
  switch (v) {
    case TypeId::Uint: return ret + "Uint";
    case TypeId::Float: return ret + "Float";
    case TypeId::String: return ret + "String";
    case TypeId::Vector: return ret + "Vector";
    default: return "ERROR";
  }
}
// clang-format on

template <typename T>
void typeChecker(T) {
  cout << __PRETTY_FUNCTION__;
  cout << " (typeid.name = " << typeid(T).name() << ")" << endl;
}

int depth = 0;
string pad() { return string(depth * 2, ' '); }

template <typename T>
class ValueStore {
public:
  ValueStore(const T &v) : val(v) {}
  T &get() { return val; }

  template <typename ElemT>
  void push_back(const ElemT &v) {
    val.push_back(v);
  }

  template <typename ElemT>
  uint64_t size() {
    return val.size();
  }

private:
  T val;
};

#if 0
class IntegerType {
public:
  IntegerType(uint64_t val) : value(val) {}
  Buffer serialize();
  string toString();

private:
  ValueStore<uint64_t> value;
};

Buffer IntegerType::serialize() { return serializeUint(value.get()); }
string IntegerType::toString() { return pad() + "IntegerType(" + to_string(value.get()) + ")\n"; }
#endif

class StringType {
public:
  StringType(string val) : value(val) {}
  Buffer serialize();
  string toString();

private:
  ValueStore<string> value;
};

Buffer StringType::serialize() {
  string tmp = value.get();
  Buffer ret = serializeUint(tmp.size());
  for (auto &ch : tmp)
    ret.push_back(static_cast<std::byte>(ch));
  return ret;
}

string StringType::toString() {
  string tmp = value.get();
  return pad() + "StringType[" + to_string(tmp.size()) + "](" + value.get() + ")\n";
}

#if 0
class FloatType {
public:
  FloatType() = default;
  FloatType(double val) : Any(TypeId::Float), value(val) {}
  Buffer serialize() override {
    Buffer ret;
    char *chPtr = reinterpret_cast<char *>(&value.get());
    for (uint64_t i = 0; i != sizeof(value.get()); ++i) {
      ret.push_back(*reinterpret_cast<std::byte *>(chPtr + i));
    }
    return Any::serialize(ret);
  }
  string toString() override { return pad() + "FloatType(" + to_string(value.get()) + ")\n"; }

private:
  ValueStore<double> value;
};
*/

#endif

class Any;

class VectorType {
public:
  VectorType() : value(ValueStore<vector<Any>>(vector<Any>())) {}
  string toString();
  Buffer serialize();

  template <typename T>
  void push_back(T &val);

  //
  //   static Buffer serialize(vector<Any *> v) {
  //     auto ret = Buffer();
  //     for (auto cur = v.begin(); cur != v.end(); ++cur) {
  //       Any *item = *cur;
  //       Buffer b = item->serialize();
  //       std::copy(b.begin(), b.end(), back_inserter(ret));
  //     }
  //     return ret;
  //   }

private:
  ValueStore<vector<Any>> value;
};

string VectorType::toString() {
  string res = pad() + "VectorType[" + to_string(value.size<Any>()) + "]\n";
  //   ++depth;
  //   for (auto &i : value.get()) {
  //     res += i->toString();
  //   }
  //   --depth;
  return res;
}

template <typename T>
void VectorType::push_back(T &val) {
  value.push_back(Any(val));
}

class Any {

public:
  Any(VectorType val) : typeId(TypeId::Vector), vVal(val) {}
  Any(StringType val) : typeId(TypeId::String), sVal(val) {}
//   Any(IntegerType val) : typeId(TypeId::Uint), iVal(val) {}
  //
  //   any(typeid tid) : typeid(tid) {}
  //   any(buffer::const_iterator &);
  //   typeid gettypeid() const { return typeid; }
  //   id gettype() const { return static_cast<id>(typeid); }
  //
  template <typename T>
  auto &getValue();

  template <TypeId kId>
  auto &getValue();

  Buffer serialize();
  string toString();

  //   Buffer serialize(Buffer val) {
  //     auto ret = Buffer();
  //     auto bTypeId = serializeUint(static_cast<uint64_t>(typeId));
  //     std::copy(bTypeId.begin(), bTypeId.end(), back_inserter(ret));
  //     std::copy(val.begin(), val.end(), back_inserter(ret));
  //     return ret;
  //   }

private:
  TypeId typeId;
//   IntegerType iVal;
  StringType sVal;
  //   FloatType fVal;
  VectorType vVal;
};

template <typename T>
auto &getValue() {}

template <TypeId kId>
auto &Any::getValue() {
  if (kId == TypeId::Vector)
    return v;
}

Buffer Any::serialize() {
  Buffer ret = serializeUint(static_cast<uint64_t>(typeId));
  Buffer bVal;
  switch (typeId) {
  case TypeId::Vector:
    auto val = getValue<TypeId::Vector>();
    bVal = val.serialize();
  }
  copy(bVal.cbegin(), bVal.cend(), back_inserter(ret));
  return ret;
}
string Any::toString() {
  string ret;
  switch (typeId) {
  case TypeId::Vector:
    auto val = getValue<TypeId::Vector>();
    ret = val.toString();
  }
  return ret;
}

Buffer VectorType::serialize() {
  auto ret = serializeUint(value.size<Any>());
  for (auto &item : value.get()) {
    auto bItem = item.serialize();
    copy(bItem.cbegin(), bItem.cend(), back_inserter(ret));
  }
  return ret;
  //   auto retItems = serialize(value.get());
  //   std::copy(retItems.begin(), retItems.end(), back_inserter(ret));
  //   return Any::serialize(ret);
}

// Any::Any(Buffer::const_iterator &iter) {
//   auto typeIdUint = deserializeUint(iter);
//   typeId = static_cast<TypeId>(typeIdUint);
// }

class Serializator {
public:
  Buffer serialize();
  string toString();

  template <typename Arg>
  void push(Arg val) {
    Any a{val};
    storage.push_back(a);
  }
  //   static vector<Any> deserialize(const Buffer &b);
private:
  vector<Any> storage;
};

Buffer Serializator::serialize() {
  auto ret = serializeUint(storage.size());
  for (auto &item : storage) {
    auto b = item.serialize();
    std::copy(b.begin(), b.end(), back_inserter(ret));
  }
  return ret;
}

string Serializator::toString() {
  std::stringstream ss;
  ss << "[" << to_string(storage.size()) << "]\n";
  for (auto &item : storage)
    ss << item.toString();
  return ss.str();
}

#if 0
vector<Any> Serializator::deserialize(const Buffer &b) {
  vector<Any> ret;
  auto iter = b.cbegin();
  auto size = deserializeUint(iter);
  for (uint64_t i = 0; i != size; ++i) {
    ret.push_back(Any(iter));
  }
  return ret;
}
#endif
#endif
