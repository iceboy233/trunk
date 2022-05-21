#include "io/posix-file.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "absl/strings/str_cat.h"

namespace io {
namespace {

absl::Span<const uint8_t> make_span(std::string_view string) {
    return {reinterpret_cast<const uint8_t *>(string.data()), string.size()};
}

absl::Span<uint8_t> make_mutable_span(std::string &string) {
    return {reinterpret_cast<uint8_t *>(string.data()), string.size()};
}

TEST(PosixFileTest, basic) {
    std::string filename = absl::StrCat(getenv("TEST_TMPDIR"), "/test");

    // Create a file and use pwrite to overwrite a portion.
    std::unique_ptr<File> file;
    std::error_code ec = open_posix_file(
        filename.c_str(), O_CREAT | O_WRONLY, 0644, file);
    ASSERT_FALSE(ec);
    size_t size;
    ec = file->write(make_span("hello "), size);
    ASSERT_FALSE(ec);
    ec = file->write(make_span("world"), size);
    ASSERT_FALSE(ec);
    ec = file->pwrite(4, make_span("O W"), size);
    ASSERT_FALSE(ec);
    int64_t file_size;
    ec = file->size(file_size);
    ASSERT_FALSE(ec);
    EXPECT_EQ(file_size, 11);
    file.reset();

    // Read the file to verify content and behavior.
    ec = open_posix_file(filename.c_str(), O_RDONLY, 0, file);
    ASSERT_FALSE(ec);
    std::string buffer(6, '\0');
    ec = file->read(make_mutable_span(buffer), size);
    ASSERT_FALSE(ec);
    ASSERT_EQ(size, 6);
    EXPECT_EQ(buffer, "hellO ");
    ec = file->read(make_mutable_span(buffer), size);
    ASSERT_FALSE(ec);
    ASSERT_EQ(size, 5);
    EXPECT_EQ(std::string_view(buffer).substr(0, 5), "World");
    ec = file->read(make_mutable_span(buffer), size);
    ASSERT_FALSE(ec);
    EXPECT_EQ(size, 0);
    int64_t position;
    ec = file->tell(position);
    ASSERT_FALSE(ec);
    ASSERT_EQ(position, 11);

    // Seek back and read again.
    ec = file->seek(5);
    ASSERT_FALSE(ec);
    ec = file->tell(position);
    ASSERT_FALSE(ec);
    ASSERT_EQ(position, 5);
    ec = file->read(make_mutable_span(buffer), size);
    ASSERT_FALSE(ec);
    ASSERT_EQ(size, 6);
    EXPECT_EQ(buffer, " World");
    ec = file->pread(2, make_mutable_span(buffer), size);
    ASSERT_FALSE(ec);
    ASSERT_EQ(size, 6);
    EXPECT_EQ(buffer, "llO Wo");
}

}  // namespace
}  // namespace io
