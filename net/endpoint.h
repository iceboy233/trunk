#ifndef _NET_ENDPOINT_H
#define _NET_ENDPOINT_H

#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "net/asio.h"
#include "util/strings.h"

namespace net {

template <typename AddressT>
class EndpointBase {
public:
    EndpointBase() = default;
    EndpointBase(const AddressT &address, uint16_t port)
        : address_(address), port_(port) {}

    const AddressT &address() const { return address_; }
    void set_address(const AddressT &address) { address_ = address; }
    uint16_t port() const { return port_; }
    void set_port(uint16_t port) { port_ = port; }

    operator tcp::endpoint() const { return {address_, port_}; }
    operator udp::endpoint() const { return {address_, port_}; }

    static std::optional<EndpointBase<AddressT>> from_string(
        std::string_view string);
    std::string to_string() const;

private:
    static std::optional<AddressT> parse_address(std::string_view string);

    AddressT address_;
    uint16_t port_;
};

template <typename AddressT>
std::optional<EndpointBase<AddressT>> EndpointBase<AddressT>::from_string(
    std::string_view string) {
    size_t pos = string.rfind(':');
    if (pos == std::string_view::npos) {
        return std::nullopt;
    }
    std::string_view address_str;
    if (pos >= 2 && string[0] == '[' && string[pos - 1] == ']') {
        address_str = string.substr(1, pos - 2);
    } else {
        address_str = string.substr(0, pos);
    }
    auto address_or = parse_address(address_str);
    if (!address_or) {
        return std::nullopt;
    }
    std::string_view port_str = string.substr(pos + 1);
    uint16_t port = util::consume_uint16(port_str);
    if (!port_str.empty()) {
        return std::nullopt;
    }
    return EndpointBase(*address_or, port);
}

template <typename A, typename B>
bool operator==(
    const EndpointBase<A> &a, const EndpointBase<B> &b) {
    return a.address() == b.address() && a.port() == b.port();
}

using Endpoint = EndpointBase<address>;
using EndpointV4 = EndpointBase<address_v4>;
using EndpointV6 = EndpointBase<address_v6>;

template <typename AddressT>
bool AbslParseFlag(
    std::string_view in, EndpointBase<AddressT> *out, std::string *error) {
    auto address_or = EndpointBase<AddressT>::from_string(in);
    if (!address_or) {
        return false;
    }
    *out = *address_or;
    return true;
}

template <typename AddressT>
std::string AbslUnparseFlag(const EndpointBase<AddressT> &in) {
    return in.to_string();
}

}  // namespace net

#endif  // _NET_ENDPOINT_H
