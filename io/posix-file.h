#ifndef _IO_POSIX_FILE_H
#define _IO_POSIX_FILE_H

#include <fcntl.h>
#include <system_error>

#include "io/file.h"

namespace io {

class PosixFile : public File {
public:
    PosixFile() : fd_(-1) {}
    explicit PosixFile(int fd) : fd_(fd) {}
    ~PosixFile() override { close(); }

    std::error_code open(const char *filename, int flags, mode_t mode);
    void close();

    std::error_code read(absl::Span<uint8_t> buffer, size_t &size) override;
    std::error_code write(
        absl::Span<const uint8_t> buffer, size_t &size) override;
    std::error_code pread(
        int64_t position, absl::Span<uint8_t> buffer, size_t &size) override;
    std::error_code pwrite(
        int64_t position,
        absl::Span<const uint8_t> buffer,
        size_t &size) override;
    std::error_code seek(int64_t position) override;
    std::error_code tell(int64_t &position) override;
    std::error_code size(int64_t &size) override;

private:
    int fd_;
};

}  // namespace io

#endif  // _IO_POSIX_FILE_H
