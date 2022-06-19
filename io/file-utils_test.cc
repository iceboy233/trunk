#include "io/file-utils.h"

#include <string_view>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "io/memory-file.h"

namespace io {
namespace {

using ::testing::ElementsAre;

TEST(FileUtilsTest, read) {
    MemoryFile file({'f', 'o', 'o', 'b', 'a', 'r'});
    std::array<char, 4> buffer;
    size_t size;
    std::error_code ec = read(file, buffer, size);
    ASSERT_FALSE(ec);
    EXPECT_EQ(std::string_view(buffer.data(), 4), "foob");
    EXPECT_EQ(size, 4);
    ec = read(file, buffer, size);
    ASSERT_FALSE(ec);
    EXPECT_EQ(std::string_view(buffer.data(), 2), "ar");
    EXPECT_EQ(size, 2);
}

TEST(FileUtilsTest, read_to_end) {
    MemoryFile file({'f', 'o', 'o', 'b', 'a', 'r'});
    std::string content;
    std::error_code ec = read_to_end(file, content);
    ASSERT_FALSE(ec);
    EXPECT_EQ(content, "foobar");
}

TEST(FileUtilsTest, write) {
    MemoryFile file;
    std::error_code ec = write(file, "foobar");
    ASSERT_FALSE(ec);
    EXPECT_THAT(file.content(), ElementsAre('f', 'o', 'o', 'b', 'a', 'r'));
}

}  // namespace
}  // namespace io
