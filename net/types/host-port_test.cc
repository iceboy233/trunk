#include "net/types/host-port.h"

#include <gtest/gtest.h>
#include <optional>

#include "absl/hash/hash_testing.h"

namespace net {
namespace {

TEST(HostPortTest, empty) {
    HostPort host_port;
    EXPECT_EQ(host_port.host(), Host());
    EXPECT_EQ(host_port.port(), 0);
    EXPECT_TRUE(host_port.is_addr_port());
    EXPECT_EQ(host_port, AddrPort());
    EXPECT_EQ(tcp::endpoint(host_port), tcp::endpoint());
    EXPECT_EQ(udp::endpoint(host_port), udp::endpoint());
    EXPECT_EQ(AddrPort(host_port), AddrPort());
    EXPECT_FALSE(host_port.is_name_port());
    EXPECT_EQ(host_port.to_string(), "0.0.0.0:0");
    EXPECT_EQ(HostPort::from_string(""), std::nullopt);
    EXPECT_EQ(HostPort::from_string("0"), std::nullopt);
    EXPECT_EQ(HostPort::from_string(":"), std::nullopt);
    EXPECT_EQ(HostPort::from_string(":0"), std::nullopt);
    EXPECT_EQ(HostPort::from_string("0.0.0.0"), std::nullopt);
    EXPECT_EQ(HostPort::from_string("0.0.0.0:"), std::nullopt);
    EXPECT_EQ(HostPort::from_string("0.0.0.0:0"), host_port);
}

TEST(HostPortTest, address_v4) {
    HostPort host_port(make_address("1.2.3.4"), 5678);
    EXPECT_EQ(host_port.host(), make_address("1.2.3.4"));
    EXPECT_EQ(host_port.port(), 5678);
    EXPECT_TRUE(host_port.is_addr_port());
    EXPECT_EQ(host_port, AddrPort(make_address("1.2.3.4"), 5678));
    EXPECT_EQ(tcp::endpoint(host_port),
              tcp::endpoint(make_address("1.2.3.4"), 5678));
    EXPECT_EQ(udp::endpoint(host_port),
              udp::endpoint(make_address("1.2.3.4"), 5678));
    EXPECT_EQ(AddrPort(host_port), AddrPort(make_address("1.2.3.4"), 5678));
    EXPECT_FALSE(host_port.is_name_port());
    EXPECT_EQ(host_port.to_string(), "1.2.3.4:5678");
    EXPECT_EQ(HostPort::from_string("1.2.3.4:5678"), host_port);
    EXPECT_EQ(HostPort::from_string("[1.2.3.4]:5678"), std::nullopt);
}

TEST(HostPortTest, address_v6) {
    HostPort host_port(make_address("2001:db8::8888"), 5678);
    EXPECT_EQ(host_port.host(), make_address("2001:db8::8888"));
    EXPECT_EQ(host_port.port(), 5678);
    EXPECT_TRUE(host_port.is_addr_port());
    EXPECT_EQ(host_port, AddrPort(make_address("2001:db8::8888"), 5678));
    EXPECT_EQ(tcp::endpoint(host_port),
              tcp::endpoint(make_address("2001:db8::8888"), 5678));
    EXPECT_EQ(udp::endpoint(host_port),
              udp::endpoint(make_address("2001:db8::8888"), 5678));
    EXPECT_EQ(AddrPort(host_port),
              AddrPort(make_address("2001:db8::8888"), 5678));
    EXPECT_FALSE(host_port.is_name_port());
    EXPECT_EQ(host_port.to_string(), "[2001:db8::8888]:5678");
    EXPECT_EQ(HostPort::from_string("[2001:db8::8888]:5678"), host_port);
    EXPECT_EQ(HostPort::from_string("2001:db8::8888:5678"), std::nullopt);
}

TEST(HostPortTest, name) {
    HostPort host_port(Host("name.test"), 5678);
    EXPECT_EQ(host_port.host(), Host("name.test"));
    EXPECT_EQ(host_port.port(), 5678);
    EXPECT_FALSE(host_port.is_addr_port());
    EXPECT_TRUE(host_port.is_name_port());
    EXPECT_EQ(host_port.name(), "name.test");
    EXPECT_EQ(host_port.to_string(), "name.test:5678");
    EXPECT_EQ(HostPort::from_string("name.test:5678"), host_port);
    EXPECT_EQ(HostPort::from_string("[name.test]:5678"), std::nullopt);
}

TEST(HostPortTest, hash) {
    EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly({
        HostPort(),
        HostPort(make_address("1.2.3.4"), 1234),
        HostPort(make_address("1.2.3.4"), 5678),
        HostPort(make_address("2001:db8::8888"), 5678),
        HostPort(Host("name.test"), 5678),
    }));
}

}  // namespace
}  // namespace net
