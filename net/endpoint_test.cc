#include "net/endpoint.h"

#include <gtest/gtest.h>
#include <optional>

namespace net {
namespace {

TEST(EndpointTest, v4) {
    Endpoint e(address_v4::from_string("1.2.3.4"), 5678);
    EndpointV4 e4(address_v4::from_string("1.2.3.4"), 5678);
    EXPECT_EQ(e, e4);
    EXPECT_EQ(e.to_string(), "1.2.3.4:5678");
    EXPECT_EQ(e4.to_string(), "1.2.3.4:5678");
    EXPECT_EQ(e, Endpoint::from_string("1.2.3.4:5678"));
    EXPECT_EQ(e, EndpointV4::from_string("1.2.3.4:5678"));
    EXPECT_EQ(Endpoint::from_string("1.2.3:5678"), std::nullopt);
    EXPECT_EQ(EndpointV4::from_string("1.2.3:5678"), std::nullopt);
    EXPECT_EQ(EndpointV6::from_string("1.2.3.4:5678"), std::nullopt);
}

TEST(EndpointTest, v6) {
    Endpoint e(address_v6::from_string("2001:db8::8888"), 5678);
    EndpointV6 e6(address_v6::from_string("2001:db8::8888"), 5678);
    EXPECT_EQ(e, e6);
    EXPECT_EQ(e.to_string(), "[2001:db8::8888]:5678");
    EXPECT_EQ(e6.to_string(), "[2001:db8::8888]:5678");
    EXPECT_EQ(e, Endpoint::from_string("[2001:db8::8888]:5678"));
    EXPECT_EQ(e, EndpointV6::from_string("[2001:db8::8888]:5678"));
    EXPECT_EQ(Endpoint::from_string("[2001:db8:8888]:5678"), std::nullopt);
    EXPECT_EQ(EndpointV6::from_string("[2001:db8:8888]:5678"), std::nullopt);
    EXPECT_EQ(EndpointV4::from_string("[2001:db8::8888]:5678"), std::nullopt);
}

}  // namespace
}  // namespace net
