#ifndef _IO_FILE_H
#define _IO_FILE_H

#include <cstddef>
#include <cstdint>
#include <system_error>

#include "absl/types/span.h"

namespace io {

class File {
public:
    File() = default;
    virtual ~File() = default;

    virtual std::error_code read(absl::Span<uint8_t> buffer, size_t &size) = 0;
    virtual std::error_code write(
        absl::Span<const uint8_t> buffer, size_t &size) = 0;
    virtual std::error_code pread(
        int64_t position, absl::Span<uint8_t> buffer, size_t &size) = 0;
    virtual std::error_code pwrite(
        int64_t position, absl::Span<const uint8_t> buffer, size_t &size) = 0;
    virtual std::error_code seek(int64_t position) = 0;
    virtual std::error_code tell(int64_t &position) = 0;
    virtual std::error_code size(int64_t &size) = 0;
};

}  // namespace io

#endif  // _IO_FILE_H
