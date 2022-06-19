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
std::error_code read_to_end(File &file, std::string &content);

// Writes the entire `buffer` into `file`.
std::error_code write(File &file, ConstBufferSpan buffer);

}  // namespace io

#endif  // _IO_FILE_UTILS_H
