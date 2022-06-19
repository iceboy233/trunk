#ifndef _IO_STREAM_H
#define _IO_STREAM_H

#include <array>
#include <ostream>
#include <streambuf>

#include "io/file.h"

namespace io {

class OStream : private std::streambuf, public std::ostream {
public:
    explicit OStream(File &file);
    ~OStream() override { sync(); }

private:
    int sync() override;
    int overflow(int c) override;

    File &file_;
    std::array<char, 8192> buffer_;
};

}  // namespace io

#endif  // _IO_STREAM_H
