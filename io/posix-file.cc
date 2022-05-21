#include "io/posix-file.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>

namespace io {
namespace {

class PosixFile : public File {
public:
    explicit PosixFile(int fd);
    ~PosixFile() override;

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

PosixFile::PosixFile(int fd)
    : fd_(fd) {}

PosixFile::~PosixFile() {
    close(fd_);
}

std::error_code PosixFile::read(absl::Span<uint8_t> buffer, size_t &size) {
    ssize_t ret = ::read(fd_, buffer.data(), buffer.size());
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<size_t>(ret);
    return {};
}

std::error_code PosixFile::write(
    absl::Span<const uint8_t> buffer, size_t &size) {
    ssize_t ret = ::write(fd_, buffer.data(), buffer.size());
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<size_t>(ret);
    return {};
}

std::error_code PosixFile::pread(
    int64_t position, absl::Span<uint8_t> buffer, size_t &size) {
    ssize_t ret = pread64(fd_, buffer.data(), buffer.size(), position);
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<size_t>(ret);
    return {};
}

std::error_code PosixFile::pwrite(
    int64_t position, absl::Span<const uint8_t> buffer, size_t &size) {
    ssize_t ret = pwrite64(fd_, buffer.data(), buffer.size(), position);
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<size_t>(ret);
    return {};
}

std::error_code PosixFile::seek(int64_t position) {
    if (lseek64(fd_, position, SEEK_SET) < 0) {
        return {errno, std::system_category()};
    }
    return {};
}

std::error_code PosixFile::tell(int64_t &position) {
    off_t ret = lseek64(fd_, 0, SEEK_CUR);
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    position = static_cast<int64_t>(ret);
    return {};
}

std::error_code PosixFile::size(int64_t &size) {
    struct stat64 stat;
    if (fstat64(fd_, &stat) < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<int64_t>(stat.st_size);
    return {};
}

}  // namespace

std::error_code open_posix_file(
    const char *filename, int flags, mode_t mode, std::unique_ptr<File> &file) {
    int fd = open(filename, flags, mode);
    if (fd < 0) {
        return {errno, std::system_category()};
    }
    file = wrap_posix_file(fd);
    return {};
}

std::unique_ptr<File> wrap_posix_file(int fd) {
    return std::make_unique<PosixFile>(fd);
}

}  // namespace io
