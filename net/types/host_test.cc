#include "net/types/host.h"

#include <gtest/gtest.h>
#include <optional>

#include "absl/hash/hash_testing.h"

namespace net {
namespace {

TEST(HostTest, empty) {
    Host host;
    EXPECT_TRUE(host.empty());
    EXPECT_TRUE(host.is_address());
    EXPECT_EQ(host.address(), address());
    EXPECT_FALSE(host.is_name());
    EXPECT_EQ(host.to_string(), "0.0.0.0");
    EXPECT_EQ(Host::from_string("0.0.0.0"), host);
    EXPECT_EQ(Host::from_string(""), std::nullopt);
}

TEST(HostTest, address_v4) {
    Host host(make_address("1.2.3.4"));
    EXPECT_FALSE(host.empty());
    EXPECT_TRUE(host.is_address());
    EXPECT_EQ(host.address(), make_address("1.2.3.4"));
    EXPECT_FALSE(host.is_name());
    EXPECT_EQ(host.to_string(), "1.2.3.4");
    EXPECT_EQ(Host::from_string("1.2.3.4"), host);
}

TEST(HostTest, address_v6) {
    Host host(make_address("2001:db8::8888"));
    EXPECT_FALSE(host.empty());
    EXPECT_TRUE(host.is_address());
    EXPECT_EQ(host.address(), make_address("2001:db8::8888"));
    EXPECT_FALSE(host.is_name());
    EXPECT_EQ(host.to_string(), "2001:db8::8888");
    EXPECT_EQ(Host::from_string("2001:db8::8888"), host);
}

TEST(HostTest, name) {
    Host host("name.test");
    EXPECT_FALSE(host.empty());
    EXPECT_FALSE(host.is_address());
    EXPECT_TRUE(host.is_name());
    EXPECT_EQ(host.name(), "name.test");
    EXPECT_EQ(host.to_string(), "name.test");
    EXPECT_EQ(Host::from_string("name.test"), host);
}

TEST(HostTest, hash) {
    EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly({
        Host(),
        Host(make_address("1.2.3.4")),
        Host(make_address("2001:db8::8888")),
        Host("name.test"),
    }));
}

}  // namespace
}  // namespace net
