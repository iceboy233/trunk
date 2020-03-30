#ifndef _UTIL_UTF_H
#define _UTIL_UTF_H

#include <string>
#include <string_view>

namespace util {

size_t utf16_to_utf8_size(std::u16string_view input);
void utf16_to_utf8(std::u16string_view input, char *output);

inline std::string utf16_to_utf8(std::u16string_view input) {
    std::string result(utf16_to_utf8_size(input), '\0');
    utf16_to_utf8(input, &result[0]);
    return result;
}

size_t utf8_to_utf16_size(std::string_view input);
void utf8_to_utf16(std::string_view input, char16_t *output);

inline std::u16string utf8_to_utf16(std::string_view input) {
    std::u16string result(utf8_to_utf16_size(input), '\0');
    utf8_to_utf16(input, &result[0]);
    return result;
}

}  // namespace util

#endif
