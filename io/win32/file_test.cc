#include "io/win32/file.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace io {
namespace win32 {
namespace {

TEST(FileTest, main) {
    std::wstring filename = std::wstring(_wgetenv(L"TEST_TMPDIR")) + L"/test";

    // Create a file and use pwrite to overwrite a portion.
    File file;
    std::error_code ec = file.create(filename.c_str());
    ASSERT_FALSE(ec);
    size_t size;
    ec = file.write("hello ", size);
    ASSERT_FALSE(ec);
    ec = file.write("world", size);
    ASSERT_FALSE(ec);
    ec = file.pwrite(4, "O W", size);
    ASSERT_FALSE(ec);
    int64_t file_size;
    ec = file.size(file_size);
    ASSERT_FALSE(ec);
    EXPECT_EQ(file_size, 11);
    file.close();

    // Read the file to verify content and behavior.
    ec = file.open(filename.c_str(), GENERIC_READ);
    ASSERT_FALSE(ec);
    std::string buffer(6, '\0');
    ec = file.read(buffer, size);
    ASSERT_FALSE(ec);
    ASSERT_EQ(size, 6);
    EXPECT_EQ(buffer, "hellO ");
    ec = file.read(buffer, size);
    ASSERT_FALSE(ec);
    ASSERT_EQ(size, 5);
    EXPECT_EQ(std::string_view(buffer).substr(0, 5), "World");
    ec = file.read(buffer, size);
    ASSERT_FALSE(ec);
    EXPECT_EQ(size, 0);
    int64_t position;
    ec = file.tell(position);
    ASSERT_FALSE(ec);
    ASSERT_EQ(position, 11);

    // Seek back and read again.
    ec = file.seek(5);
    ASSERT_FALSE(ec);
    ec = file.tell(position);
    ASSERT_FALSE(ec);
    ASSERT_EQ(position, 5);
    ec = file.read(buffer, size);
    ASSERT_FALSE(ec);
    ASSERT_EQ(size, 6);
    EXPECT_EQ(buffer, " World");
    ec = file.pread(2, buffer, size);
    ASSERT_FALSE(ec);
    ASSERT_EQ(size, 6);
    EXPECT_EQ(buffer, "llO Wo");
}

}  // namespace
}  // namespace win32
}  // namespace io
