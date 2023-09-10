#include "util/utf.h"

#include <gtest/gtest.h>

namespace util {
namespace {

constexpr std::u16string_view utf16 =
    u"\u3053\u3093\u306b\u3061\u306f\u3001\u4e16\u754c";
constexpr std::string_view utf8 =
    "\xe3\x81\x93\xe3\x82\x93\xe3\x81\xab\xe3\x81\xa1\xe3\x81\xaf\xe3\x80\x81"
    "\xe4\xb8\x96\xe7\x95\x8c";

TEST(UtfTest, utf16_to_utf8) {
    EXPECT_EQ(utf16_to_utf8(u"hello world"), "hello world");
    EXPECT_EQ(utf16_to_utf8(utf16), utf8);
}

TEST(UtfTest, utf8_to_utf16) {
    EXPECT_EQ(utf8_to_utf16("hello world"), u"hello world");
    EXPECT_EQ(utf8_to_utf16(utf8), utf16);
}

}  // namespace
}  // namespace util
