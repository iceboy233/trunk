#include "io/posix/file.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>

namespace io {
namespace posix {

SharedFile std_input(STDIN_FILENO);
SharedFile std_output(STDOUT_FILENO);
SharedFile std_error(STDERR_FILENO);

std::error_code SharedFile::read(BufferSpan buffer, size_t &size) {
    ssize_t ret = ::read(fd_, buffer.data(), buffer.size());
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<size_t>(ret);
    return {};
}

std::error_code SharedFile::write(ConstBufferSpan buffer, size_t &size) {
    ssize_t ret = ::write(fd_, buffer.data(), buffer.size());
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<size_t>(ret);
    return {};
}

std::error_code SharedFile::pread(
    int64_t position, BufferSpan buffer, size_t &size) {
    ssize_t ret = pread64(fd_, buffer.data(), buffer.size(), position);
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<size_t>(ret);
    return {};
}

std::error_code SharedFile::pwrite(
    int64_t position, ConstBufferSpan buffer, size_t &size) {
    ssize_t ret = pwrite64(fd_, buffer.data(), buffer.size(), position);
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<size_t>(ret);
    return {};
}

std::error_code SharedFile::seek(int64_t position) {
    if (lseek64(fd_, position, SEEK_SET) < 0) {
        return {errno, std::system_category()};
    }
    return {};
}

std::error_code SharedFile::tell(int64_t &position) {
    off_t ret = lseek64(fd_, 0, SEEK_CUR);
    if (ret < 0) {
        return {errno, std::system_category()};
    }
    position = static_cast<int64_t>(ret);
    return {};
}

std::error_code SharedFile::size(int64_t &size) {
    struct stat64 stat;
    if (fstat64(fd_, &stat) < 0) {
        return {errno, std::system_category()};
    }
    size = static_cast<int64_t>(stat.st_size);
    return {};
}

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

}  // namespace posix
}  // namespace io
