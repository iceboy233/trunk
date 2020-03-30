#include "util/utf.h"

namespace util {

size_t utf16_to_utf8_size(std::u16string_view input) {
    size_t output_size = 0;
    while (!input.empty()) {
        char16_t c = input[0];
        if (c <= 0x7f) {
            output_size += 1;
        } else if (c <= 0x7ff) {
            output_size += 2;
        } else {
            output_size += 3;
        }
        input.remove_prefix(1);
    }
    return output_size;
}

void utf16_to_utf8(std::u16string_view input, char *output) {
    while (!input.empty()) {
        char16_t c = input[0];
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
        input.remove_prefix(1);
    }
}

size_t utf8_to_utf16_size(std::string_view input) {
    size_t output_size = 0;
    while (!input.empty()) {
        char c = input[0];
        if ((c & 0xc0) != 0xc0) {
            input.remove_prefix(1);
        } else if ((c & 0xe0) != 0xe0) {
            if (input.size() < 2) {
                break;
            }
            input.remove_prefix(2);
        } else if ((c & 0xf0) != 0xf0) {
            if (input.size() < 3) {
                break;
            }
            input.remove_prefix(3);
        } else {
            if (input.size() < 4) {
                break;
            }
            input.remove_prefix(4);
        }
        ++output_size;
    }
    return output_size;
}

void utf8_to_utf16(std::string_view input, char16_t *output) {
    while (!input.empty()) {
        char c = input[0];
        if ((c & 0xc0) != 0xc0) {
            *output++ = static_cast<char16_t>(c & 0x7f);
            input.remove_prefix(1);
        } else if ((c & 0xe0) != 0xe0) {
            if (input.size() < 2) {
                break;
            }
            *output++ = (static_cast<char16_t>(c & 0x3f) << 6) |
                        static_cast<char16_t>(input[1] & 0x3f);
            input.remove_prefix(2);
        } else if ((c & 0xf0) != 0xf0) {
            if (input.size() < 3) {
                break;
            }
            *output++ = (static_cast<char16_t>(c & 0x1f) << 12) |
                        (static_cast<char16_t>(input[1] & 0x3f) << 6) |
                        static_cast<char16_t>(input[2] & 0x3f);
            input.remove_prefix(3);
        } else {
            if (input.size() < 4) {
                break;
            }
            *output++ = u'\ufffd';
            input.remove_prefix(4);
        }
    }
}

}  // namespace util
