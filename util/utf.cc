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

size_t utf8_to_utf16_size(const char *input, size_t input_size) {
    size_t output_size = 0;
    while (input_size) {
        char c = *input;
        if ((c & 0xc0) != 0xc0) {
            ++input;
            --input_size;
        } else if ((c & 0xe0) != 0xe0) {
            if (input_size < 2) {
                break;
            }
            input += 2;
            input_size -= 2;
        } else {
            if (input_size < 3) {
                break;
            }
            input += 3;
            input_size -= 3;
        }
        ++output_size;
    }
    return output_size;
}

void utf8_to_utf16(const char *input, size_t input_size, char16_t *output) {
    while (input_size) {
        char c = *input;
        if ((c & 0xc0) != 0xc0) {
            *output++ = static_cast<char16_t>(c & 0x7f);
            ++input;
            --input_size;
        } else if ((c & 0xe0) != 0xe0) {
            if (input_size < 2) {
                break;
            }
            *output++ = (static_cast<char16_t>(c & 0x3f) << 6) |
                        static_cast<char16_t>(input[1] & 0x3f);
            input += 2;
            input_size -= 2;
        } else {
            if (input_size < 3) {
                break;
            }
            *output++ = (static_cast<char16_t>(c & 0x1f) << 12) |
                        (static_cast<char16_t>(input[1] & 0x3f) << 6) |
                        static_cast<char16_t>(input[2] & 0x3f);
            input += 3;
            input_size -= 3;
        }
    }
}

}  // namespace util
