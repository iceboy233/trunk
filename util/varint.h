#ifndef _UTIL_VARINT_H
#define _UTIL_VARINT_H

#include <cstddef>
#include <cstdint>
#include <vector>
#include "absl/types/span.h"

namespace util {

template <typename T>
void append_varint(T value, std::vector<uint8_t> &output) {
    while (true) {
        uint8_t cur = static_cast<uint8_t>(value & 0x7f);
        value >>= 7;
        if (!value) {
            output.push_back(cur);
            return;
        }
        output.push_back(cur | 0x80);
    }
}

template <typename T>
bool parse_varint(absl::Span<const uint8_t> &input, T &output) {
    T result = 0;
    size_t size = 0;
    while (size < input.size()) {
        uint8_t cur = input[size];
        result |= (cur & 0x7f) << (size * 7);
        ++size;
        if (!(cur & 0x80)) {
            input.remove_prefix(size);
            output = result;
            return true;
        }
    }
    return false;
}

}  // namespace util

#endif  // _UTIL_VARINT_H
