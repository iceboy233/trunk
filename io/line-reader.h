#ifndef _IO_LINE_READER_H
#define _IO_LINE_READER_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <system_error>
#include <vector>

#include "io/file.h"

namespace io {

class LineReader {
public:
    struct Options {
        char delimiter = '\n';
        size_t buffer_size = 8192;
    };

    LineReader(io::File &file, const Options &options);

    LineReader(const LineReader &) = delete;
    LineReader &operator=(const LineReader &) = delete;

    std::error_code read(std::string &line, bool &end_of_file);

private:
    io::File &file_;
    char delimiter_;
    std::vector<uint8_t> buffer_;
    size_t offset_ = 0;
};

}  // namespace io

#endif  // _IO_LINE_READER_H
