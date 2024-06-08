#ifndef _IO_NATIVE_FILE_H
#define _IO_NATIVE_FILE_H

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

}  // namespace io

#endif  // _IO_NATIVE_FILE_H
