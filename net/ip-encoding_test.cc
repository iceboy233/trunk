#include "net/ip-encoding.h"

#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "net/asio.h"

namespace net {
namespace {

using ::testing::ElementsAre;

TEST(IpEncoding, encode) {
    std::vector<uint8_t> bytes;
    encode(address::from_string("127.0.0.1"), bytes);
    EXPECT_THAT(bytes, ElementsAre(127, 0, 0, 1));
    encode(address::from_string("::1"), bytes);
    EXPECT_THAT(
        bytes, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1));
    encode(address_v4::from_string("127.0.0.1"), bytes);
    EXPECT_THAT(bytes, ElementsAre(127, 0, 0, 1));
    encode(address_v6::from_string("::1"), bytes);
    EXPECT_THAT(
        bytes, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1));
}

TEST(IpEncoding, decode) {
    address address;
    address_v4 address_v4;
    address_v6 address_v6;
    ASSERT_TRUE(decode({127, 0, 0, 1}, address));
    EXPECT_EQ(address, address::from_string("127.0.0.1"));
    ASSERT_TRUE(decode({127, 0, 0, 1}, address_v4));
    EXPECT_EQ(address_v4, address_v4::from_string("127.0.0.1"));
    ASSERT_FALSE(decode({127, 0, 0, 1}, address_v6));
    ASSERT_TRUE(decode(
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, address));
    EXPECT_EQ(address, address::from_string("::1"));
    ASSERT_FALSE(decode(
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, address_v4));
    ASSERT_TRUE(decode(
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, address_v6));
    EXPECT_EQ(address_v6, address_v6::from_string("::1"));
}

}  // namespace
}  // namespace net
