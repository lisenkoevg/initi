#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

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

template <typename T>
class ValueStore {
public:
  ValueStore(const T &v) : val(v) {}
  T &get() { return val; }

  template <typename ElemT>
  void push_back(const ElemT &v) { val.push_back(v); }

  template <typename ElemT>
  size_t size() { return val.size(); }
private:
  T val;
};

class Serializator;
class Any {
  friend class Serializator;

public:
  Any(TypeId tid) : typeId(tid) {}
  TypeId getTypeId() const { return typeId; }
  Id getType() const { return static_cast<Id>(typeId); }

protected:
  Buffer serialize(Buffer val) {
    auto ret = Buffer();
    auto bTypeId = serializeUint(static_cast<uint64_t>(typeId));
    std::copy(bTypeId.begin(), bTypeId.end(), back_inserter(ret));
    std::copy(val.begin(), val.end(), back_inserter(ret));
    return ret;
  }

  static Buffer serialize(Any *item);

private:
  TypeId typeId;
};

class IntegerType : public Any {
public:
  IntegerType(uint64_t val) : Any(TypeId::Uint), value(val) {}
  Buffer serialize() { return Any::serialize(serializeUint(value.get())); }

private:
  ValueStore<uint64_t> value;
};

class StringType : public Any {
public:
  StringType(string val) : Any(TypeId::String), value(val) {}
  Buffer serialize() {
    string tmp = value.get();
    Buffer ret = serializeUint(tmp.size());
    std::for_each(tmp.begin(), tmp.end(), [&ret](auto v) { ret.push_back(static_cast<std::byte>(v)); });
    return Any::serialize(ret);
  }

private:
  ValueStore<string> value;
};

class FloatType : public Any {
public:
  FloatType(double val) : Any(TypeId::Float), value(val) {}
  Buffer serialize() {
    Buffer ret;
    char *chPtr = reinterpret_cast<char *>(&value.get());
    for (size_t i = 0; i != sizeof(value.get()); ++i) {
      ret.push_back(*reinterpret_cast<std::byte *>(chPtr + i));
    }
    return Any::serialize(ret);
  }

private:
  ValueStore<double> value;
};

class VectorType : public Any {
public:
  VectorType() : Any(TypeId::Vector), value(vector<Any *>()) {}

  Buffer serialize() {
    auto ret = serializeUint(value.size<Any>());
    auto retItems = serialize(value.get());
    std::copy(retItems.begin(), retItems.end(), back_inserter(ret));
    return Any::serialize(ret);
  }

  void push_back(Any &val) {
    value.push_back(&val);
  }

  static Buffer serialize(vector<Any *> v) {
    auto ret = Buffer();
    for (auto cur = v.begin(); cur != v.end(); ++cur) {
      Any *item = *cur;
      Buffer b = Any::serialize(item);
      std::copy(b.begin(), b.end(), back_inserter(ret));
    }
    return ret;
  }

private:
  ValueStore<vector<Any *>> value;
};

Buffer Any::serialize(Any *item) {
  Buffer b;
  if (item->getTypeId() == TypeId::Uint) {
    b = static_cast<IntegerType *>(item)->serialize();
  } else if (item->getTypeId() == TypeId::Float) {
    b = static_cast<FloatType *>(item)->serialize();
  } else if (item->getTypeId() == TypeId::String) {
    b = static_cast<StringType *>(item)->serialize();
  } else if (item->getTypeId() == TypeId::Vector) {
    b = static_cast<VectorType *>(item)->serialize();
  }
  return b;
}

class Serializator {
public:
  void push(Any &a) { storage.push_back(&a); }

  Buffer serialize() {
    auto ret = serializeUint(storage.size());
    for_each(storage.begin(), storage.end(), [&ret](Any *item) {
      Buffer b = Any::serialize(item);
      std::copy(b.begin(), b.end(), back_inserter(ret));
    });
    return ret;
  }

private:
  vector<Any *> storage;
};

void dumpUint(uint64_t val) {
  for (size_t i = 0; i < sizeof(val); ++i) {
    if (i != 0)
      cout << " ";
    std::byte *b = reinterpret_cast<std::byte *>(&val);
    cout << std::hex << std::setw(2) << std::setfill('0') << (0xff & static_cast<char>(*(b + i)));
  }
  cout << endl;
}

void dumpBuffer(const std::vector<std::byte> &b) {
  for (size_t i = 0; i != b.size(); ++i) {
    if (i != 0) {
      if (i % 8 != 0)
        cout << " ";
      else
        cout << endl;
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
