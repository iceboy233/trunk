#include "net/endpoint.h"

#include "absl/strings/str_cat.h"

namespace net {

template <>
std::string EndpointBase<address>::to_string() const {
    if (address_.is_v4()) {
        return absl::StrCat(address_.to_string(), ":", port_);
    } else {
        return absl::StrCat("[", address_.to_string(), "]:", port_);
    }
}

template <>
std::string EndpointBase<address_v4>::to_string() const {
    return absl::StrCat(address_.to_string(), ":", port_);
}

template <>
std::string EndpointBase<address_v6>::to_string() const {
    return absl::StrCat("[", address_.to_string(), "]:", port_);
}

template <>
std::optional<address> EndpointBase<address>::parse_address(
    std::string_view string) {
    boost::system::error_code ec;
    auto address = make_address(string, ec);
    if (ec) {
        return std::nullopt;
    }
    return address;
}

template <>
std::optional<address_v4> EndpointBase<address_v4>::parse_address(
    std::string_view string) {
    boost::system::error_code ec;
    auto address = make_address_v4(string, ec);
    if (ec) {
        return std::nullopt;
    }
    return address;
}

template <>
std::optional<address_v6> EndpointBase<address_v6>::parse_address(
    std::string_view string) {
    boost::system::error_code ec;
    auto address = make_address_v6(string, ec);
    if (ec) {
        return std::nullopt;
    }
    return address;
}

}  // namespace net
