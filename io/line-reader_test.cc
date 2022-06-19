#include "io/line-reader.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "io/memory-file.h"

namespace io {
namespace {

TEST(LineReaderTest, main) {
    MemoryFile file({'h', 'e', 'l', 'l', 'o', '\n',
                     'w', 'o', 'r', 'l', 'd', '\n'});
    LineReader reader(file, {});
    std::string line;
    bool end_of_file;
    std::error_code ec = reader.read(line, end_of_file);
    ASSERT_FALSE(ec);
    ASSERT_FALSE(end_of_file);
    EXPECT_EQ(line, "hello\n");
    ec = reader.read(line, end_of_file);
    ASSERT_FALSE(ec);
    ASSERT_FALSE(end_of_file);
    EXPECT_EQ(line, "world\n");
    ec = reader.read(line, end_of_file);
    ASSERT_FALSE(ec);
    ASSERT_TRUE(end_of_file);
}

}  // namespace
}  // namespace io
