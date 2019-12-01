#ifndef _UTIL_UTF_H
#define _UTIL_UTF_H

#include <string>

namespace util {

size_t utf16_to_utf8_size(const char16_t *input, size_t input_size);
void utf16_to_utf8(const char16_t *input, size_t input_size, char *output);

inline std::string utf16_to_utf8(const char16_t *input, size_t input_size) {
    std::string result(utf16_to_utf8_size(input, input_size), '\0');
    utf16_to_utf8(input, input_size, &result[0]);
    return result;
}

}  // namespace util

#endif
