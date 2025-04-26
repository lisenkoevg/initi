#ifndef TYPES_H
#define TYPES_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <cstdint>

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::to_string;
using std::vector;
using std::copy;

using Id = uint64_t;
using Buffer = std::vector<std::byte>;
enum class TypeId : Id { Uint, Float, String, Vector };

#endif
