#ifndef _UTIL_STRINGS_H
#define _UTIL_STRINGS_H

#include <cstdint>
#include <string_view>

namespace util {

int8_t consume_int8(std::string_view &in);
uint8_t consume_uint8(std::string_view &in);
int16_t consume_int16(std::string_view &in);
uint16_t consume_uint16(std::string_view &in);
int32_t consume_int32(std::string_view &in);
uint32_t consume_uint32(std::string_view &in);
int64_t consume_int64(std::string_view &in);
uint64_t consume_uint64(std::string_view &in);

}  // namespace util

#endif  // _UTIL_STRINGS_H
