#ifndef _UTIL_IOSTREAM_H
#define _UTIL_IOSTREAM_H

#include <istream>
#include <ostream>
#include <string>
#include <string_view>

namespace util {

bool read(std::istream &in, std::string &out);
bool write(std::string_view in, std::ostream &out);

}  // namespace util

#endif  // _UTIL_IOSTREAM_H
