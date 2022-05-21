#ifndef _IO_POSIX_FILE_H
#define _IO_POSIX_FILE_H

#include <fcntl.h>
#include <memory>
#include <system_error>

#include "io/file.h"

namespace io {

std::error_code open_posix_file(
    const char *filename, int flags, mode_t mode, std::unique_ptr<File> &file);
std::unique_ptr<File> wrap_posix_file(int fd);

}  // namespace io

#endif  // _IO_POSIX_FILE_H
