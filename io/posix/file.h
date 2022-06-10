#ifndef _IO_POSIX_FILE_H
#define _IO_POSIX_FILE_H

#include <fcntl.h>

#include "io/file.h"

namespace io {
namespace posix {

class SharedFile : public io::File {
public:
    explicit SharedFile(int fd) : fd_(fd) {}
    ~SharedFile() override = default;

    std::error_code read(BufferSpan buffer, size_t &size) override;
    std::error_code write(ConstBufferSpan buffer, size_t &size) override;
    std::error_code pread(
        int64_t position, BufferSpan buffer, size_t &size) override;
    std::error_code pwrite(
        int64_t position, ConstBufferSpan buffer, size_t &size) override;
    std::error_code seek(int64_t position) override;
    std::error_code tell(int64_t &position) override;
    std::error_code size(int64_t &size) override;

protected:
    int fd_;
};

extern SharedFile stdin;
extern SharedFile stdout;
extern SharedFile stderr;

class File : public SharedFile {
public:
    File() : SharedFile(-1) {}
    explicit File(int fd) : SharedFile(fd) {}
    ~File() override { close(); }

    File(const File &) = delete;
    File &operator=(const File &) = delete;

    std::error_code create(const char *filename) {
        return open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    }
    std::error_code open(const char *filename, int flags, mode_t mode = 0);

    void close();
};

}  // namespace posix
}  // namespace io

#endif  // _IO_POSIX_FILE_H
