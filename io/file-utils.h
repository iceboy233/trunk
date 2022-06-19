#ifndef _IO_FILE_UTILS_H
#define _IO_FILE_UTILS_H

#include <string>
#include <system_error>

#include "io/file.h"

namespace io {

// Reads eagerly from `file` to `buffer`. `size` smaller than buffer size
// indicates end of file is reached.
std::error_code read(File &file, BufferSpan buffer, size_t &size);

// Reads from `file` until end of file is reached.
template <typename T, size_t buffer_size = 8192>
std::error_code read_to_end(File &file, T &content);

// Writes the entire `buffer` into `file`.
std::error_code write(File &file, ConstBufferSpan buffer);

template <typename T, size_t buffer_size>
std::error_code read_to_end(File &file, T &content) {
    content.clear();
    size_t pos = 0;
    size_t size;
    do {
        content.resize(pos + buffer_size);
        std::error_code ec = file.read({&content[pos], buffer_size}, size);
        if (ec) {
            return ec;
        }
        pos += size;
        content.resize(pos);
    } while (size);
    return {};
}

}  // namespace io

#endif  // _IO_FILE_UTILS_H
