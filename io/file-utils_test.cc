#include "io/file-utils.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "io/memory-file.h"

namespace io {
namespace {

TEST(FileUtilsTest, read_to_end) {
    MemoryFile file({'f', 'o', 'o', 'b', 'a', 'r'});
    std::string content;
    std::error_code ec = read_to_end(file, content);
    ASSERT_FALSE(ec);
    EXPECT_EQ(content, "foobar");
}

}  // namespace
}  // namespace io
