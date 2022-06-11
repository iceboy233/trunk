#ifndef _IO_FILE_UTILS_H
#define _IO_FILE_UTILS_H

#include <string>
#include <system_error>

#include "io/file.h"

namespace io {

std::error_code read_to_end(File &file, std::string &content);

}  // namespace io

#endif  // _IO_FILE_UTILS_H
