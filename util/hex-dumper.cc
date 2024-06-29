#include "util/hex-dumper.h"

#include <algorithm>

#include "io/file-utils.h"

namespace util {
namespace {

inline char to_hex(int value) {
    return value < 10 ? '0' + value : 'a' + value - 10;
}

inline char to_char(int value) {
    return value >= 32 && value <= 127 ? value : '.';
}

void put_uint8_hex(uint8_t value, char output[2]) {
    output[0] = to_hex(value >> 4);
    output[1] = to_hex(value & 0xf);
}

void put_uint32_hex(uint32_t value, char output[8]) {
    for (int i = 0; i < 8; ++i) {
        output[i] = to_hex(value >> 28);
        value <<= 4;
    }
}

}  // namespace

HexDumper::HexDumper(int width)
    : width_(width),
      buffer_(char_start() + width_ + 1, ' ') {}

std::error_code HexDumper::dump(
    uint32_t offset, ConstBufferSpan input, io::File &output) {
    while (input.size() >= width_) {
        std::error_code ec = dump_line(offset, input, output);
        if (ec) {
            return ec;
        }
        offset += width_;
        input.remove_prefix(width_);
    }
    if (!input.empty()) {
        std::error_code ec = dump_line(offset, input, output);
        if (ec) {
            return ec;
        }
    }
    return {};
}

std::error_code HexDumper::dump_line(
    uint32_t offset, ConstBufferSpan input, io::File &output) {
    put_uint32_hex(offset, &buffer_[0]);
    buffer_[8] = ':';
    const size_t size = std::min(input.size(), width_);
    size_t i;
    for (i = 0; i < size; ++i) {
        put_uint8_hex(input[i], &buffer_[i * 5 / 2 + 10]);
        buffer_[char_start() + i] = to_char(input[i]);
    }
    for (; i < width_; ++i) {
        buffer_[i * 5 / 2 + 10] = ' ';
        buffer_[i * 5 / 2 + 11] = ' ';
    }
    buffer_[char_start() + size] = '\n';
    return io::write(output, {buffer_.data(), char_start() + size + 1});
}

}  // namespace util
