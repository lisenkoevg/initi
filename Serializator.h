#ifndef SERIALIZATOR_H
#define SERIALIZATOR_H

#include "types.h"
#include "utils.h"

#define PR (cout << __PRETTY_FUNCTION__ << endl);

string toString(TypeId);

string toString(TypeId v) {
  string ret = "TypeId::";
  switch (v) {
  case TypeId::Uint:
    return ret + "Uint";
  case TypeId::Float:
    return ret + "Float";
  case TypeId::String:
    return ret + "String";
  case TypeId::Vector:
    return ret + "Vector";
  default:
    return ret;
  }
}

template <typename T>
void typeChecker(T &&t) {
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
  using value_type = uint64_t;
  IntegerType(value_type val = 0) : value(val) {}
  Buffer serialize();
  string toString();
  static IntegerType deserialize(Buffer::const_iterator &);

private:
  ValueStore<value_type> value;
};

Buffer IntegerType::serialize() { return serializeUint(value.get()); }
string IntegerType::toString() { return pad() + "IntegerType(" + to_string(value.get()) + ")\n"; }
IntegerType IntegerType::deserialize(Buffer::const_iterator &iter) {
  auto ret = deserializeUint(iter);
  return IntegerType(ret);
}

class StringType {
public:
  StringType(string val = "") : value(val) {}
  Buffer serialize();
  string toString();

  static StringType deserialize(Buffer::const_iterator &);

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
  return pad() + "StringType[" + to_string(tmp.size()) + "](" + tmp + ")\n";
}

StringType StringType::deserialize(Buffer::const_iterator &iter) {
  auto size = deserializeUint(iter);
  string ret = "";
  for (uint64_t i = 0; i != size; ++i, ++iter) {
    auto ch = static_cast<char>(*iter);
    ret.push_back(ch);
  }
  return StringType(ret);
}

class FloatType {
public:
  using value_type = double;
  FloatType(value_type val = 0) : value(val) {}
  Buffer serialize();
  string toString();

  static FloatType deserialize(Buffer::const_iterator &);

private:
  ValueStore<value_type> value;
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

FloatType FloatType::deserialize(Buffer::const_iterator &iter) {
  FloatType::value_type ret;
  for (uint64_t i = 0; i != sizeof(ret); ++i, ++iter) {
    auto ch = static_cast<char>(*iter);
    *(reinterpret_cast<char *>(&ret) + i) = ch;
  }
  return FloatType{ret};
}

class Any;

class VectorType {
public:
  VectorType() : value(ValueStore<vector<Any>>(vector<Any>())) {}

  static VectorType deserialize(Buffer::const_iterator &);

  string toString();
  Buffer serialize();

  template <typename T>
  void push_back(T &val);

  Any &operator[](uint64_t idx);

private:
  ValueStore<vector<Any>> value;
};

class Any {

public:
  Any(VectorType val) : typeId(TypeId::Vector), vVal(val) {}
  Any(StringType val) : typeId(TypeId::String), sVal(val) {}
  Any(IntegerType val) : typeId(TypeId::Uint), iVal(val) {}
  Any(FloatType val) : typeId(TypeId::Float), fVal(val) {}

  // How these two getValue() methods below should look like?
  template <typename T>
  auto &getValue();

  template <TypeId kId>
  auto &getValue();

  Buffer serialize();
  string toString();

  TypeId getPayloadTypeId() const;

  static Any deserialize(Buffer::const_iterator &iter);

private:
  TypeId typeId;

  // memory waste (union?)
  IntegerType iVal;
  StringType sVal;
  FloatType fVal;
  VectorType vVal;
};

template <typename T>
auto &getValue() {}

template <TypeId kId>
auto &Any::getValue() {}

// How to get rid of switch operator in serialize(), deserialize() and toString() methods below?

Buffer Any::serialize() {
  Buffer ret = serializeUint(static_cast<uint64_t>(typeId));
  Buffer tmp;
  switch (typeId) {
  case TypeId::Vector:
    tmp = vVal.serialize();
    break;
  case TypeId::Uint:
    tmp = iVal.serialize();
    break;
  case TypeId::String:
    tmp = sVal.serialize();
    break;
  case TypeId::Float:
    tmp = fVal.serialize();
    break;
  }
  copy(tmp.cbegin(), tmp.cend(), back_inserter(ret));
  return ret;
}

string Any::toString() {
  string tmp;
  switch (typeId) {
  case TypeId::Vector:
    tmp = vVal.toString();
    break;
  case TypeId::Uint:
    tmp = iVal.toString();
    break;
  case TypeId::String:
    tmp = sVal.toString();
    break;
  case TypeId::Float:
    tmp = fVal.toString();
    break;
  }
  return tmp;
}

TypeId Any::getPayloadTypeId() const { return typeId; }

Any Any::deserialize(Buffer::const_iterator &iter) {
  auto typeIdInt = deserializeUint(iter);
  auto typeId = static_cast<TypeId>(typeIdInt);
  switch (typeId) {
  case TypeId::Vector: {
    auto val = VectorType::deserialize(iter);
    return Any{val};
  }
  case TypeId::String: {
    auto val = StringType::deserialize(iter);
    return Any(val);
  }
  case TypeId::Uint: {
    auto val = IntegerType::deserialize(iter);
    return Any(val);
  }
  case TypeId::Float: {
    auto val = FloatType::deserialize(iter);
    return Any(val);
  }
  default:
    cout << "Error: wrong typeId = " << typeIdInt << endl;
    abort();
  }
}

VectorType VectorType::deserialize(Buffer::const_iterator &iter) {
  VectorType ret;
  auto size = deserializeUint(iter);
  for (uint64_t i = 0; i != size; ++i) {
    Any a = Any::deserialize(iter);
    ret.push_back(a);
  }
  return ret;
}

template <typename T>
void VectorType::push_back(T &val) {
  Any tmp{val};
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

Any &VectorType::operator[](uint64_t idx) { return (value.get())[idx]; }

class Serializator {
public:
  Buffer serialize();
  string toString();

  template <typename Arg>
  void push(Arg &&val);
  static vector<Any> deserialize(const Buffer &b);

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

vector<Any> Serializator::deserialize(const Buffer &b) {
  vector<Any> ret;
  auto iter = b.cbegin();
  auto size = deserializeUint(iter);
  for (uint64_t i = 0; i != size; ++i) {
    auto a = Any::deserialize(iter);
    ret.push_back(a);
  }
  return ret;
}

#endif
