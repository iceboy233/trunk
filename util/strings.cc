#include "util/strings.h"

namespace util {

template <typename T>
T consume_int(std::string_view &in);

int8_t consume_int8(std::string_view &in) {
    return consume_int<int8_t>(in);
}

uint8_t consume_uint8(std::string_view &in) {
    return consume_int<uint8_t>(in);
}

int16_t consume_int16(std::string_view &in) {
    return consume_int<int16_t>(in);
}

uint16_t consume_uint16(std::string_view &in) {
    return consume_int<uint16_t>(in);
}

int32_t consume_int32(std::string_view &in) {
    return consume_int<int32_t>(in);
}

uint32_t consume_uint32(std::string_view &in) {
    return consume_int<uint32_t>(in);
}

int64_t consume_int64(std::string_view &in) {
    return consume_int<int64_t>(in);
}

uint64_t consume_uint64(std::string_view &in) {
    return consume_int<uint64_t>(in);
}

template <typename T>
T consume_int(std::string_view &in) {
    bool negate = false;
    if (!in.empty() && in.front() == '-') {
        in.remove_prefix(1);
        negate = true;
    }
    T result = 0;
    while (!in.empty() && in.front() >= '0' && in.front() <= '9') {
        result = result * 10 + (in.front() - '0');
        in.remove_prefix(1);
    }
    return !negate ? result : -result;
}

}  // namespace util
