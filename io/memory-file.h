#ifndef _IO_MEMORY_FILE_H
#define _IO_MEMORY_FILE_H

#include <cstddef>
#include <vector>

#include "io/file.h"

namespace io {

class MemoryFile : public File {
public:
    MemoryFile() = default;
    explicit MemoryFile(std::vector<uint8_t> content);

    std::error_code read(BufferSpan buffer, size_t &size) override;
    std::error_code write(ConstBufferSpan buffer, size_t &size) override;
    std::error_code pread(
        int64_t position, BufferSpan buffer, size_t &size) override;
    std::error_code pwrite(
        int64_t position, ConstBufferSpan buffer, size_t &size) override;
    std::error_code seek(int64_t position) override;
    std::error_code tell(int64_t &position) override;
    std::error_code size(int64_t &size) override;

private:
    std::vector<uint8_t> content_;
    size_t position_ = 0;
};

}  // namespace io

#endif  // _IO_MEMORY_FILE_H
