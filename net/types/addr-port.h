#ifndef _NET_TYPES_ADDR_PORT_H
#define _NET_TYPES_ADDR_PORT_H

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include "net/asio.h"
#include "net/asio-hash.h"

namespace net {

class AddrPort {
public:
    AddrPort() = default;
    AddrPort(const net::address &address, uint16_t port)
        : address_(address), port_(port) {}

    const net::address &address() const { return address_; }
    uint16_t port() const { return port_; }

    void set_address(const net::address &address) { address_ = address; }
    void set_port(uint16_t port) { port_ = port; }

    operator tcp::endpoint() const { return tcp::endpoint(address_, port_); }
    operator udp::endpoint() const { return udp::endpoint(address_, port_); }

    std::string to_string() const;
    static std::optional<AddrPort> from_string(std::string_view string);

private:
    friend bool operator==(const AddrPort &a, const AddrPort &b);
    friend bool operator!=(const AddrPort &a, const AddrPort &b);

    template <typename H>
    friend H AbslHashValue(H h, const AddrPort &addr_port);

    net::address address_;
    uint16_t port_ = 0;
};

template <typename H>
H AbslHashValue(H h, const AddrPort &addr_port) {
    return H::combine(std::move(h), addr_port.address_, addr_port.port_);
}

bool AbslParseFlag(std::string_view in, AddrPort *out, std::string *error);
std::string AbslUnparseFlag(const AddrPort &in);

}  // namespace net

#endif  // _NET_TYPES_ADDR_PORT_H
