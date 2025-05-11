#include "net/types/addr-port.h"

#include <gtest/gtest.h>
#include <optional>

#include "absl/hash/hash_testing.h"

namespace net {
namespace {

TEST(AddrPortTest, empty) {
    AddrPort addr_port;
    EXPECT_EQ(addr_port.address(), address());
    EXPECT_EQ(addr_port.port(), 0);
    EXPECT_EQ(addr_port.to_string(), "0.0.0.0:0");
    EXPECT_EQ(AddrPort::from_string(""), std::nullopt);
    EXPECT_EQ(AddrPort::from_string("0"), std::nullopt);
    EXPECT_EQ(AddrPort::from_string(":"), std::nullopt);
    EXPECT_EQ(AddrPort::from_string(":0"), std::nullopt);
    EXPECT_EQ(AddrPort::from_string("0.0.0.0"), std::nullopt);
    EXPECT_EQ(AddrPort::from_string("0.0.0.0:"), std::nullopt);
    EXPECT_EQ(AddrPort::from_string("0.0.0.0:0"), addr_port);
}

TEST(AddrPortTest, address_v4) {
    AddrPort addr_port(make_address("1.2.3.4"), 5678);
    EXPECT_EQ(addr_port.address(), make_address("1.2.3.4"));
    EXPECT_EQ(addr_port.port(), 5678);
    EXPECT_EQ(addr_port.to_string(), "1.2.3.4:5678");
    EXPECT_EQ(AddrPort::from_string("1.2.3.4:5678"), addr_port);
    EXPECT_EQ(AddrPort::from_string("1.2.3:5678"), std::nullopt);
    EXPECT_EQ(AddrPort::from_string("[1.2.3.4]:5678"), std::nullopt);
}

TEST(AddrPortTest, address_v6) {
    AddrPort addr_port(make_address("2001:db8::8888"), 5678);
    EXPECT_EQ(addr_port.address(), make_address("2001:db8::8888"));
    EXPECT_EQ(addr_port.port(), 5678);
    EXPECT_EQ(addr_port.to_string(), "[2001:db8::8888]:5678");
    EXPECT_EQ(AddrPort::from_string("[2001:db8::8888]:5678"), addr_port);
    EXPECT_EQ(AddrPort::from_string("[2001:db8:8888]:5678"), std::nullopt);
    EXPECT_EQ(AddrPort::from_string("2001:db8::8888:5678"), std::nullopt);
}

TEST(AddrPortTest, hash) {
    EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly({
        AddrPort(),
        AddrPort(make_address("1.2.3.4"), 1234),
        AddrPort(make_address("1.2.3.4"), 5678),
        AddrPort(make_address("2001:db8::8888"), 5678),
    }));
}

}  // namespace
}  // namespace net
