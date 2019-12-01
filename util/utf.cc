#include "util/utf.h"

namespace util {

size_t utf16_to_utf8_size(const char16_t *input, size_t input_size) {
    size_t output_size = 0;
    while (input_size) {
        char16_t c = *input;
        if (c <= 0x7f) {
            output_size += 1;
        } else if (c <= 0x7ff) {
            output_size += 2;
        } else {
            output_size += 3;
        }
        ++input;
        --input_size;
    }
    return output_size;
}

void utf16_to_utf8(const char16_t *input, size_t input_size, char *output) {
    while (input_size) {
        char16_t c = *input;
        if (c <= 0x7f) {
            *output++ = static_cast<char>(c);
        } else if (c <= 0x7ff) {
            *output++ = static_cast<char>((c >> 6) | 0xc0);
            *output++ = static_cast<char>((c & 0x3f) | 0x80);
        } else {
            *output++ = static_cast<char>((c >> 12) | 0xe0);
            *output++ = static_cast<char>(((c >> 6) & 0x3f) | 0x80);
            *output++ = static_cast<char>((c & 0x3f) | 0x80);
        }
        ++input;
        --input_size;
    }
}

}  // namespace util
