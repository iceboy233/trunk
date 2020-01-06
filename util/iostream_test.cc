#include "util/iostream.h"

#include <sstream>
#include <gtest/gtest.h>

namespace util {
namespace {

TEST(IostreamTest, read) {
    std::istringstream in("foo");
    std::string out;
    ASSERT_TRUE(read(in, out));
    EXPECT_EQ(out, "foo");
}

TEST(IostreamTest, read_fail) {
    std::istringstream in;
    in.setstate(std::ios::failbit);
    std::string out;
    ASSERT_FALSE(read(in, out));
}

TEST(IostreamTest, write) {
    std::ostringstream out;
    ASSERT_TRUE(write("foo", out));
    EXPECT_EQ(out.str(), "foo");
}

TEST(IostreamTest, write_fail) {
    std::ostringstream out;
    out.setstate(std::ios::failbit);
    ASSERT_FALSE(write("foo", out));
}

}  // namespace
}  // namespace util
