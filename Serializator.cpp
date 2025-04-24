#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#define PR (cout << __PRETTY_FUNCTION__ << endl);

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::to_string;
using std::vector;

using Id = uint64_t;
using Buffer = vector<std::byte>;

enum class TypeId : Id { Uint, Float, String, Vector };
string toString(TypeId);

void dumpUint(uint64_t val);
void dumpBuffer(const std::vector<std::byte> &b);
void writeBufferToFile(const Buffer &b, const string &filename);

Buffer serializeUint(uint64_t val);

// Read uint64_t start from given iterator and advance iterator forward by sizeof(uint64_t)
uint64_t deserializeUint(Buffer::const_iterator &);

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

class Serializator;

class Any {
  friend class Serializator;

public:
  Any() {}
  Any(TypeId tid) : typeId(tid) {}
  Any(Buffer::const_iterator &);
  TypeId getTypeId() const { return typeId; }
  Id getType() const { return static_cast<Id>(typeId); }

  virtual Buffer serialize() {
    PR;
    return Buffer();
  }
  virtual string toString() {
    PR;
    return string();
  }

  ~Any() {}

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

Any::Any(Buffer::const_iterator &iter) {
  auto typeIdUint = deserializeUint(iter);
  typeId = static_cast<TypeId>(typeIdUint);
}

class IntegerType : public Any {
public:
  IntegerType(uint64_t val) : Any(TypeId::Uint), value(val) {}
  Buffer serialize() override { return Any::serialize(serializeUint(value.get())); }
  string toString() override { return pad() + "IntegerType(" + to_string(value.get()) + ")\n"; }

private:
  ValueStore<uint64_t> value;
};

class StringType : public Any {
public:
  StringType(string val) : Any(TypeId::String), value(val) {}
  Buffer serialize() override {
    string tmp = value.get();
    Buffer ret = serializeUint(tmp.size());
    std::for_each(tmp.begin(), tmp.end(), [&ret](auto v) { ret.push_back(static_cast<std::byte>(v)); });
    return Any::serialize(ret);
  }
  string toString() override {
    string tmp = value.get();
    return pad() + "StringType[" + to_string(tmp.size()) + "](" + value.get() + ")\n";
  }

private:
  ValueStore<string> value;
};

class FloatType : public Any {
public:
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

class VectorType : public Any {
public:
  VectorType() : Any(TypeId::Vector), value(vector<Any *>()) {}

  Buffer serialize() override {
    auto ret = serializeUint(value.size<Any>());
    auto retItems = serialize(value.get());
    std::copy(retItems.begin(), retItems.end(), back_inserter(ret));
    return Any::serialize(ret);
  }

  string toString() override {
    string res = pad() + "VectorType[" + to_string(value.size<Any>()) + "]\n";
    ++depth;
    for (auto &i : value.get()) {
      res += i->toString();
    }
    --depth;
    return res;
  }

  void push_back(Any &val) { value.push_back(&val); }

  static Buffer serialize(vector<Any *> v) {
    auto ret = Buffer();
    for (auto cur = v.begin(); cur != v.end(); ++cur) {
      Any *item = *cur;
      Buffer b = item->serialize();
      std::copy(b.begin(), b.end(), back_inserter(ret));
    }
    return ret;
  }

private:
  ValueStore<vector<Any *>> value;
};

class Serializator {
public:
  void push(Any &a) { storage.push_back(&a); }

  Buffer serialize() {
    auto ret = serializeUint(storage.size());
    for_each(storage.begin(), storage.end(), [&ret](Any *item) {
      Buffer b = item->serialize();
      std::copy(b.begin(), b.end(), back_inserter(ret));
    });
    return ret;
  }

  string toString() {
    std::stringstream ss;
    ss << "[" << to_string(storage.size()) << "]\n";
    for (auto &i : storage)
      ss << i->toString();
    return ss.str();
  }

  static vector<Any> deserialize(const Buffer &b);

private:
  vector<Any *> storage;
};

vector<Any> Serializator::deserialize(const Buffer &b) {
  vector<Any> ret;
  auto iter = b.cbegin();
  auto size = deserializeUint(iter);
  for (uint64_t i = 0; i != size; ++i) {
    ret.push_back(Any(iter));
  }
  return ret;
}

Buffer serializeUint(uint64_t val) {
  std::byte *b = reinterpret_cast<std::byte *>(&val);
  uint64_t sz = sizeof(val);
  auto ret = Buffer();
  for (uint64_t i = 0; i != sz; ++i) {
    ret.push_back(*(b + i));
  }
  return ret;
}

uint64_t deserializeUint(Buffer::const_iterator &iter) {
  uint64_t ret = 0;
  for (uint64_t i = 0; i != sizeof(uint64_t); ++i, ++iter)
    *(reinterpret_cast<std::byte *>(&ret) + i) = *iter;
  return ret;
}

void dumpUint(uint64_t val) {
  for (uint64_t i = 0; i < sizeof(val); ++i) {
    if (i != 0)
      cout << " ";
    std::byte *b = reinterpret_cast<std::byte *>(&val);
    cout << std::hex << std::setw(2) << std::setfill('0') << (0xff & static_cast<char>(*(b + i)));
  }
  cout << endl;
}

void dumpBuffer(const std::vector<std::byte> &b) {
  for (uint64_t i = 0; i != b.size(); ++i) {
    if (i != 0) {
      if (i % 8 != 0)
        cout << " ";
      else {
        cout << endl;
        if (i % 64 == 0)
          cout << endl;
      }
    }
    cout << std::hex << std::setw(2) << std::setfill('0') << (0xff & static_cast<char>(b[i]));
  }
  cout << endl;
}

void writeBufferToFile(const Buffer &b, const string &filename) {
  std::ofstream f = std::ofstream(filename, std::ios::out | std::ios::binary);
  f.write(reinterpret_cast<const char *>(b.data()), b.size());
  f.close();
}

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
