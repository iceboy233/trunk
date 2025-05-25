#include "io/native-file.h"

#include <string>

#ifdef _WIN32
#include "util/utf.h"
#endif

namespace io {

std::error_code open(NativeFile &file, std::string_view filename) {
#ifndef _WIN32
    return file.open(std::string(filename).c_str(), O_RDONLY);
#else
    return file.open(
        reinterpret_cast<LPCWSTR>(util::utf8_to_utf16(filename).c_str()),
        GENERIC_READ);
#endif
}

std::error_code create(NativeFile &file, std::string_view filename) {
#ifndef _WIN32
    return file.create(std::string(filename).c_str());
#else
    return file.create(
        reinterpret_cast<LPCWSTR>(util::utf8_to_utf16(filename).c_str()));
#endif
}

}  // namespace io
