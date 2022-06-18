#include "io/file-utils.h"

namespace io {

std::error_code read(
    File &file, BufferSpan buffer, size_t &size, bool &end_of_file) {
    size = 0;
    while (!buffer.empty()) {
        size_t read_size;
        std::error_code ec = file.read(buffer, read_size);
        if (ec) {
            return ec;
        }
        if (!read_size) {
            end_of_file = true;
            return {};
        }
        buffer.remove_prefix(read_size);
        size += read_size;
    }
    end_of_file = false;
    return {};
}

std::error_code read_to_end(File &file, std::string &content) {
    content.clear();
    size_t pos = 0;
    size_t size;
    do {
        content.resize(pos + 8192);
        std::error_code ec = file.read({&content[pos], 8192}, size);
        if (ec) {
            return ec;
        }
        pos += size;
        content.resize(pos);
    } while (size);
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
