#include "util/strings.h"

#include <gtest/gtest.h>

namespace util {
namespace {

TEST(StringsTest, consume_int) {
    std::string_view a = "123456";
    EXPECT_EQ(consume_int32(a), 123456);
    EXPECT_TRUE(a.empty());

    a = "-123456";
    EXPECT_EQ(consume_int32(a), -123456);
    EXPECT_TRUE(a.empty());

    a = "123456abc";
    EXPECT_EQ(consume_int32(a), 123456);
    EXPECT_EQ(a, "abc");

    a = "123456";
    EXPECT_EQ(consume_uint16(a), 57920);
    EXPECT_TRUE(a.empty());
}

}  // namespace
}  // namespace util
