#ifndef _IO_WIN32_FILE_H
#define _IO_WIN32_FILE_H

#include <windows.h>

#include "io/file.h"

namespace io {
namespace win32 {

class SharedFile : public io::File {
public:
    explicit SharedFile(HANDLE handle) : handle_(handle) {}
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
    HANDLE handle_;
};

extern SharedFile std_input;
extern SharedFile std_output;
extern SharedFile std_error;

class File : public SharedFile {
public:
    File() : SharedFile(INVALID_HANDLE_VALUE) {}
    explicit File(HANDLE handle) : SharedFile(handle) {}
    ~File() override { close(); }

    File(const File &) = delete;
    File &operator=(const File &) = delete;

    std::error_code create(
        LPCWSTR filename,
        DWORD desired_access,
        DWORD share_mode,
        LPSECURITY_ATTRIBUTES security_attributes,
        DWORD create_disposition,
        DWORD flags);

    std::error_code create(
        LPCWSTR filename,
        DWORD desired_access = GENERIC_READ | GENERIC_WRITE,
        DWORD share_mode = FILE_SHARE_READ) {
        return create(
            filename,
            desired_access,
            share_mode,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL);
    }

    std::error_code open(
        LPCWSTR filename,
        DWORD desired_access,
        DWORD share_mode = FILE_SHARE_READ) {
        return create(
            filename,
            desired_access,
            share_mode,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL);
    }

    void close();
};

}  // namespace win32
}  // namespace io

#endif  // _IO_WIN32_FILE_H
