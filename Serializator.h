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
void typeChecker(T&& t) {
  cout << __PRETTY_FUNCTION__;
  cout << " (typeid.name = " << typeid(t).name() << ")" << endl;
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

class IntegerType {
public:
  IntegerType(uint64_t val = 0) : value(val) {}
  Buffer serialize();
  string toString();

private:
  ValueStore<uint64_t> value;
};

Buffer IntegerType::serialize() { return serializeUint(value.get()); }
string IntegerType::toString() { return pad() + "IntegerType(" + to_string(value.get()) + ")\n"; }

class StringType {
public:
  StringType(string val = "") : value(val) {}
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

class FloatType {
public:
  FloatType(double val = 0) : value(val) {}
  Buffer serialize();
  string toString();

private:
  ValueStore<double> value;
};

Buffer FloatType::serialize() {
  Buffer ret;
  auto tmp = value.get();
  char *chPtr = reinterpret_cast<char *>(&tmp);
  for (uint64_t i = 0; i != sizeof(tmp); ++i) {
    ret.push_back(*reinterpret_cast<std::byte *>(chPtr + i));
  }
  return ret;
}

string FloatType::toString() { return pad() + "FloatType(" + to_string(value.get()) + ")\n"; }

class Any;

class VectorType {
public:
  VectorType() : value(ValueStore<vector<Any>>(vector<Any>())) {}
  string toString();
  Buffer serialize();

  template <typename T>
  void push_back(T &val);

private:
  ValueStore<vector<Any>> value;
};

class Any {

public:
  Any(VectorType val) : typeId(TypeId::Vector), vVal(val) {}
  Any(StringType val) : typeId(TypeId::String), sVal(val) {}
  Any(IntegerType val) : typeId(TypeId::Uint), iVal(val) {}
  Any(FloatType val) : typeId(TypeId::Float), fVal(val) {}

  template <typename T>
  auto &getValue();

  template <TypeId kId>
  auto &getValue();

  Buffer serialize();
  string toString();

  TypeId getPayloadTypeId() const;

private:
  TypeId typeId;
  IntegerType iVal;
  StringType sVal;
  FloatType fVal;
  VectorType vVal;
};

template <typename T>
auto &getValue() {}

template <TypeId kId>
auto &Any::getValue() {
  if (kId == TypeId::Vector)
    return vVal;
}

Buffer Any::serialize() {
  Buffer ret = serializeUint(static_cast<uint64_t>(typeId));
  Buffer tmp;
  // clang-format off
  switch (typeId) {
    case TypeId::Vector:
      tmp = vVal.serialize(); break;
    case TypeId::Uint:
      tmp = iVal.serialize(); break;
    case TypeId::String:
      tmp = sVal.serialize(); break;
    case TypeId::Float:
      tmp = fVal.serialize(); break;
  }
  // clang-format on
  copy(tmp.cbegin(), tmp.cend(), back_inserter(ret));
  return ret;
}

string Any::toString() {
  string tmp;
  // clang-format off
  switch (typeId) {
    case TypeId::Vector:
      tmp = vVal.toString(); break;
    case TypeId::Uint:
      tmp = iVal.toString(); break;
    case TypeId::String:
      tmp = sVal.toString(); break;
    case TypeId::Float:
      tmp = fVal.toString(); break;
  }
  // clang-format on
  return tmp;
}

TypeId Any::getPayloadTypeId() const { return typeId; }

template <typename T>
void VectorType::push_back(T &val) {
  Any tmp = Any(val);
  value.push_back(tmp);
}

string VectorType::toString() {
  string res = pad() + "VectorType[" + to_string(value.size<Any>()) + "]\n";
  ++depth;
  for (auto &item : value.get()) {
    res += item.toString();
  }
  --depth;
  return res;
}

Buffer VectorType::serialize() {
  auto ret = serializeUint(value.size<Any>());
  for (auto &item : value.get()) {
    auto bItem = item.serialize();
    copy(bItem.cbegin(), bItem.cend(), back_inserter(ret));
  }
  return ret;
}

class Serializator {
public:
  Buffer serialize();
  string toString();

  template <typename Arg>
  void push(Arg &&val);
  //   static vector<Any> deserialize(const Buffer &b);
private:
  vector<Any> storage;
};

template <typename Arg>
void Serializator::push(Arg &&val) {
  Any a{val};
  storage.push_back(a);
}

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
  for (auto &item : storage) {
    ss << item.toString();
  }
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
