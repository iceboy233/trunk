#include "net/asio-flags.h"

#include <string>
#include <string_view>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace net {
namespace {

constexpr std::string_view ip_v4 = "172.16.254.1";
constexpr std::string_view ip_v6 = "2001:db8:85a3::8a2e:370:7334";

TEST(AsioFlagsTest, parse) {
    boost::asio::ip::address address;
    std::string error;
    ASSERT_TRUE(AbslParseFlag("", &address, &error));
    EXPECT_EQ(address, boost::asio::ip::address());
    EXPECT_EQ(error, "");
    ASSERT_TRUE(AbslParseFlag(ip_v4, &address, &error));
    EXPECT_EQ(address, boost::asio::ip::make_address(ip_v4));
    EXPECT_EQ(error, "");
    ASSERT_TRUE(AbslParseFlag(ip_v6, &address, &error));
    EXPECT_EQ(address, boost::asio::ip::make_address(ip_v6));
    EXPECT_EQ(error, "");
    ASSERT_FALSE(AbslParseFlag("bad", &address, &error));
    EXPECT_NE(error, "");
}

TEST(AsioFlagsTest, parse_v4) {
    boost::asio::ip::address_v4 address;
    std::string error;
    ASSERT_TRUE(AbslParseFlag("", &address, &error));
    EXPECT_EQ(address, boost::asio::ip::address_v4());
    EXPECT_EQ(error, "");
    ASSERT_TRUE(AbslParseFlag(ip_v4, &address, &error));
    EXPECT_EQ(address, boost::asio::ip::make_address_v4(ip_v4));
    EXPECT_EQ(error, "");
    ASSERT_FALSE(AbslParseFlag(ip_v6, &address, &error));
    EXPECT_NE(error, "");
    ASSERT_FALSE(AbslParseFlag("bad", &address, &error));
    EXPECT_NE(error, "");
}

TEST(AsioFlagsTest, parse_v6) {
    boost::asio::ip::address_v6 address;
    std::string error;
    ASSERT_TRUE(AbslParseFlag("", &address, &error));
    EXPECT_EQ(address, boost::asio::ip::address_v6());
    EXPECT_EQ(error, "");
    ASSERT_FALSE(AbslParseFlag(ip_v4, &address, &error));
    EXPECT_NE(error, "");
    error.clear();
    ASSERT_TRUE(AbslParseFlag(ip_v6, &address, &error));
    EXPECT_EQ(address, boost::asio::ip::make_address_v6(ip_v6));
    EXPECT_EQ(error, "");
    ASSERT_FALSE(AbslParseFlag("bad", &address, &error));
    EXPECT_NE(error, "");
}

TEST(AsioFlagsTest, unparse) {
    EXPECT_EQ(AbslUnparseFlag(boost::asio::ip::address()), "0.0.0.0");
    EXPECT_EQ(AbslUnparseFlag(boost::asio::ip::make_address(ip_v4)), ip_v4);
    EXPECT_EQ(AbslUnparseFlag(boost::asio::ip::make_address(ip_v6)), ip_v6);
}

TEST(AsioFlagsTest, unparse_v4) {
    EXPECT_EQ(AbslUnparseFlag(boost::asio::ip::address_v4()), "0.0.0.0");
    EXPECT_EQ(AbslUnparseFlag(boost::asio::ip::make_address_v4(ip_v4)), ip_v4);
}

TEST(AsioFlagsTest, unparse_v6) {
    EXPECT_EQ(AbslUnparseFlag(boost::asio::ip::address_v6()), "::");
    EXPECT_EQ(AbslUnparseFlag(boost::asio::ip::make_address_v6(ip_v6)), ip_v6);
}

}  // namespace
}  // namespace net
