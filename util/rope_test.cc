#include "util/rope.h"

#include <gtest/gtest.h>

namespace util {
namespace {

TEST(RopeTest, hello) {
    Rope rope;
    rope.insert(0, "hello ");
    rope.insert(6, " world");
    rope.erase(5, 1);
    char buffer[11];
    rope.copy(0, 11, buffer);
    EXPECT_EQ(std::string_view(buffer, 11), "hello world");
}

}  // namespace
}  // namespace util
