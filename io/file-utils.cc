#include "io/file-utils.h"

namespace io {

std::error_code read(File &file, BufferSpan buffer, size_t &size) {
    size = 0;
    while (!buffer.empty()) {
        size_t read_size;
        std::error_code ec = file.read(buffer, read_size);
        if (ec) {
            return ec;
        }
        if (!read_size) {
            return {};
        }
        buffer.remove_prefix(read_size);
        size += read_size;
    }
    return {};
}

std::error_code write(File &file, ConstBufferSpan buffer) {
    while (!buffer.empty()) {
        size_t size;
        std::error_code ec = file.write(buffer, size);
        if (ec) {
            return ec;
        }
        buffer.remove_prefix(size);
    }
    return {};
}

}  // namespace io
