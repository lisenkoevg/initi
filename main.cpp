#include "Serializator.h"
#include <chrono>

using std::chrono::duration;
auto now = std::chrono::high_resolution_clock::now;

int main() {
  std::ifstream raw;
  raw.open("raw.bin", std::ios_base::in | std::ios_base::binary);
  if (!raw.is_open())
    return 1;
  raw.seekg(0, std::ios_base::end);
  std::streamsize size = raw.tellg();
  raw.seekg(0, std::ios_base::beg);

  Buffer buff(size);
  raw.read(reinterpret_cast<char *>(buff.data()), size);

  auto t1 = now();
  auto res = Serializator::deserialize(buff);
  auto t2 = now();

  duration<double, std::milli> ms_double = t2 - t1;
  cout << "Deserialize time: " << ms_double.count() << "ms" << endl;

  Serializator s;
  for (auto &&i : res)
    s.push(i);

  auto t3 = now();
  std::cout << (buff == s.serialize()) << '\n';
  auto t4 = now();
  ms_double = t4 - t3;

  cout << "Serialize time: " << ms_double.count() << "ms" << endl;

  //   cout << s.toString() << endl;
  return 0;
}
