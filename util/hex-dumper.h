#ifndef _UTIL_HEX_DUMPER_H
#define _UTIL_HEX_DUMPER_H

#include <cstddef>
#include <cstdint>
#include <system_error>

#include "absl/container/fixed_array.h"
#include "base/types.h"
#include "io/file.h"

namespace util {

class HexDumper {
public:
    explicit HexDumper(int width = 16);

    std::error_code dump(
        uint32_t offset, ConstBufferSpan input, io::File &output);
    std::error_code dump_line(
        uint32_t offset, ConstBufferSpan input, io::File &output);

private:
    size_t char_start() const { return (width_ * 5 + 23) / 2; }

    const size_t width_;
    absl::FixedArray<char, 80> buffer_;
};

}  // namespace util

#endif  // _UTIL_HEX_DUMPER_H
