#include "io/win32/file.h"

namespace io {
namespace win32 {

SharedFile &std_input() {
    static SharedFile result(GetStdHandle(STD_INPUT_HANDLE));
    return result;
}

SharedFile &std_output() {
    static SharedFile result(GetStdHandle(STD_OUTPUT_HANDLE));
    return result;
}

SharedFile &std_error() {
    static SharedFile result(GetStdHandle(STD_ERROR_HANDLE));
    return result;
}

std::error_code SharedFile::read(BufferSpan buffer, size_t &size) {
    DWORD bytes_read;
    if (!ReadFile(
        handle_, buffer.data(), buffer.size(), &bytes_read, nullptr)) {
        return {static_cast<int>(GetLastError()), std::system_category()};
    }
    size = bytes_read;
    return {};
}

std::error_code SharedFile::write(ConstBufferSpan buffer, size_t &size) {
    DWORD bytes_written;
    if (!WriteFile(
        handle_, buffer.data(), buffer.size(), &bytes_written, nullptr)) {
        return {static_cast<int>(GetLastError()), std::system_category()};
    }
    size = bytes_written;
    return {};
}

std::error_code SharedFile::pread(
    int64_t position, BufferSpan buffer, size_t &size) {
    OVERLAPPED overlapped{};
    overlapped.Offset = position;
    overlapped.OffsetHigh = position >> 32;
    DWORD bytes_read;
    if (!ReadFile(
        handle_, buffer.data(), buffer.size(), &bytes_read, &overlapped)) {
        return {static_cast<int>(GetLastError()), std::system_category()};
    }
    size = bytes_read;
    return {};
}

std::error_code SharedFile::pwrite(
    int64_t position, ConstBufferSpan buffer, size_t &size) {
    OVERLAPPED overlapped{};
    overlapped.Offset = position;
    overlapped.OffsetHigh = position >> 32;
    DWORD bytes_written;
    if (!WriteFile(
        handle_, buffer.data(), buffer.size(), &bytes_written, &overlapped)) {
        return {static_cast<int>(GetLastError()), std::system_category()};
    }
    size = bytes_written;
    return {};
}

std::error_code SharedFile::seek(int64_t position) {
    LARGE_INTEGER file_pointer;
    file_pointer.QuadPart = position;
    if (!SetFilePointerEx(handle_, file_pointer, nullptr, FILE_BEGIN)) {
        return {static_cast<int>(GetLastError()), std::system_category()};
    }
    return {};
}

std::error_code SharedFile::tell(int64_t &position) {
    LARGE_INTEGER file_pointer;
    if (!SetFilePointerEx(handle_, {}, &file_pointer, FILE_CURRENT)) {
        return {static_cast<int>(GetLastError()), std::system_category()};
    }
    position = file_pointer.QuadPart;
    return {};
}

std::error_code SharedFile::size(int64_t &size) {
    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(handle_, &file_size)) {
        return {static_cast<int>(GetLastError()), std::system_category()};
    }
    size = file_size.QuadPart;
    return {};
}

std::error_code File::create(
    LPCWSTR filename,
    DWORD desired_access,
    DWORD share_mode,
    LPSECURITY_ATTRIBUTES security_attributes,
    DWORD create_disposition,
    DWORD flags) {
    close();
    HANDLE handle = CreateFileW(
        filename,
        desired_access,
        share_mode,
        security_attributes,
        create_disposition,
        flags,
        NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        return {static_cast<int>(GetLastError()), std::system_category()};
    }
    handle_ = handle;
    return {};
}

void File::close() {
    if (handle_ == INVALID_HANDLE_VALUE) {
        return;
    }
    CloseHandle(handle_);
    handle_ = INVALID_HANDLE_VALUE;
}

}  // namespace win32
}  // namespace io
