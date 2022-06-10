#ifndef _IO_FILE_H
#define _IO_FILE_H

#include <cstddef>
#include <cstdint>
#include <system_error>

#include "base/types.h"

namespace io {

class File {
public:
    File() = default;
    virtual ~File() = default;

    virtual std::error_code read(BufferSpan buffer, size_t &size) = 0;
    virtual std::error_code write(ConstBufferSpan buffer, size_t &size) = 0;
    virtual std::error_code pread(
        int64_t position, BufferSpan buffer, size_t &size) = 0;
    virtual std::error_code pwrite(
        int64_t position, ConstBufferSpan buffer, size_t &size) = 0;
    virtual std::error_code seek(int64_t position) = 0;
    virtual std::error_code tell(int64_t &position) = 0;
    virtual std::error_code size(int64_t &size) = 0;
};

}  // namespace io

#endif  // _IO_FILE_H
