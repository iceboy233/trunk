#include "io/posix/file.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>

namespace io {
namespace posix {

std::error_code File::open(const char *filename, int flags, mode_t mode) {
    close();
    int fd = ::open(filename, flags, mode);
    if (fd < 0) {
        return {errno, std::system_category()};
    }
    fd_ = fd;
    return {};
}

void File::close() {
    if (fd_ < 0) {
        return;
    }
    ::close(fd_);
    fd_ = -1;
}

std::error_code File::read(absl::Span<uint8_t> buffer, size_t &size) {
    ssize_t ret = ::read(fd_, buffer.data(), buffer.size());
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<size_t>(ret);
    return {};
}

std::error_code File::write(
    absl::Span<const uint8_t> buffer, size_t &size) {
    ssize_t ret = ::write(fd_, buffer.data(), buffer.size());
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<size_t>(ret);
    return {};
}

std::error_code File::pread(
    int64_t position, absl::Span<uint8_t> buffer, size_t &size) {
    ssize_t ret = pread64(fd_, buffer.data(), buffer.size(), position);
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<size_t>(ret);
    return {};
}

std::error_code File::pwrite(
    int64_t position, absl::Span<const uint8_t> buffer, size_t &size) {
    ssize_t ret = pwrite64(fd_, buffer.data(), buffer.size(), position);
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<size_t>(ret);
    return {};
}

std::error_code File::seek(int64_t position) {
    if (lseek64(fd_, position, SEEK_SET) < 0) {
        return {errno, std::system_category()};
    }
    return {};
}

std::error_code File::tell(int64_t &position) {
    off_t ret = lseek64(fd_, 0, SEEK_CUR);
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    position = static_cast<int64_t>(ret);
    return {};
}

std::error_code File::size(int64_t &size) {
    struct stat64 stat;
    if (fstat64(fd_, &stat) < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<int64_t>(stat.st_size);
    return {};
}

}  // namespace posix
}  // namespace io
