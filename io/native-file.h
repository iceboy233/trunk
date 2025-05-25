#ifndef _IO_NATIVE_FILE_H
#define _IO_NATIVE_FILE_H

#include <string_view>
#include <system_error>

#ifndef _WIN32
#include "io/posix/file.h"
#else
#include "io/win32/file.h"
#endif

namespace io {

#ifndef _WIN32
using NativeSharedFile = posix::SharedFile;
using NativeFile = posix::File;

using posix::std_input;
using posix::std_output;
using posix::std_error;
#else
using NativeSharedFile = win32::SharedFile;
using NativeFile = win32::File;

using win32::std_input;
using win32::std_output;
using win32::std_error;
#endif

// Opens the file in read-only mode.
std::error_code open(NativeFile &file, std::string_view filename);

// Opens the file in read-write mode. If the file does not exist, a new file
// will be created. Otherwise, the existing file will be truncated.
std::error_code create(NativeFile &file, std::string_view filename);

}  // namespace io

#endif  // _IO_NATIVE_FILE_H
