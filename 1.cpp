#include <iostream>
#include <vector>
#include <fstream>

using std::cout;
using std::endl;
using std::is_same;
using std::string;
using std::vector;
using std::ostream;

using Id = uint64_t;
using Buffer = std::vector<std::byte>;

enum class TypeId : Id { Uint, Float, String, Vector };
constexpr int enumSize = 4;

string enumToStr(TypeId v) {
  switch (v) {
  case TypeId::Uint:
    return "Uint";
    break;
  case TypeId::Vector:
    return "Vector";
    break;
  default:
    std::cerr << "Wrong enum value" << endl;
    abort();
    break;
  }
}

class Any {
public:
  template <typename... Args>
  Any(Args &&...);

  void serialize(Buffer _buff) const;

  Buffer::const_iterator deserialize(Buffer::const_iterator _begin, Buffer::const_iterator _end);

  TypeId getPayloadTypeId() const;

  template <typename Type>
  auto &getValue() const;

  bool operator==(const Any &_o) const;
};

class IntegerType {
  friend ostream& operator<<(ostream&, const IntegerType&);
public:
  IntegerType(unsigned v) : val(v) {}
private:
  unsigned val;
};

ostream & operator<<(ostream &os, const IntegerType& v) {
  return os << v.val;
}

class VectorType {
public:
  VectorType() : val() {}

  void push_back(_v) {
    val.push_back(_v);
  }

private:
  vector<unsigned> val;
};

class Serializator {
public:
  template <typename Arg>
  void push(Arg &&_val) {}

  Buffer serialize() const;

  static vector<Any> deserialize(const Buffer &);

  const vector<Any> getStorage() const { return storage; }

private:
  vector<Any> storage;
};

Buffer Serializator::serialize() const { return Buffer(); }

vector<Any> Serializator::deserialize(const Buffer &_val) { return vector<Any>(); }

void writeTestPacket() {
     VectorType v;
//      v.push_back(StringType("qwerty"));
     v.push_back(IntegerType(100500));
  //   Serializator s;
  //   s.push(v);
  //   Buffer b = s.serialize();
  //   cout << b.size() << endl;
  //   std::ofstream("raw_test.bin", std::ios::out | std::ios::binary).write(reinterpret_cast<char *>(&b),
  //   b.size());
}

int main() {
  writeTestPacket();
#if 0
  std::ifstream raw;
  raw.open("raw.bin", std::ios_base::in | std::ios_base::binary);
  if (!raw.is_open())
    return EXIT_FAILURE;
  raw.seekg(0, std::ios_base::end);
  std::streamsize size = raw.tellg();
  raw.seekg(0, std::ios_base::beg);

  Buffer buff(size);
  raw.read(reinterpret_cast<char *>(buff.data()), size);
  cout << size << " " << (size == 37144322) << endl;

  auto res = Serializator::deserialize(buff);
  Serializator s;
  for (auto &&i : res)
    s.push(i);
  std::cout << (buff == s.serialize()) << endl;

  std::cout << "TypeId::Uint: " << static_cast<Id>(TypeId::Uint) << endl;
  std::cout << "TypeId::Vector: " << static_cast<Id>(TypeId::Vector) << endl;

  return EXIT_SUCCESS;
#endif
}
