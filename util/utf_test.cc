#include "util/utf.h"

#include <gtest/gtest.h>

namespace util {
namespace {

TEST(UtfTest, utf16_to_utf8) {
    EXPECT_EQ(utf16_to_utf8(u"hello world", 11), "hello world");
    EXPECT_EQ(
        utf16_to_utf8(u"\u4f60\u597d\uff0c\u4e16\u754c", 5),
        "\xe4\xbd\xa0\xe5\xa5\xbd\xef\xbc\x8c\xe4\xb8\x96\xe7\x95\x8c");
}

}  // namespace
}  // namespace util
