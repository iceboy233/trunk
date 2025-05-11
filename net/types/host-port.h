#ifndef _NET_TYPES_HOST_PORT_H
#define _NET_TYPES_HOST_PORT_H

#include <cstdint>
#include <optional>
#include <string>

#include "net/asio.h"
#include "net/types/host.h"

namespace net {

class HostPort {
public:
    HostPort() = default;
    HostPort(const Host &host, uint16_t port) : host_(host), port_(port) {}

    Host &host() { return host_; }
    const Host &host() const { return host_; }
    uint16_t port() const { return port_; }
    void set_port(uint16_t port) { port_ = port; }

    bool is_addr_port() const { return host_.is_address(); }
    const net::address &address() const { return host_.address(); }
    explicit operator tcp::endpoint() const { return {host_.address(), port_}; }
    explicit operator udp::endpoint() const { return {host_.address(), port_}; }

    bool is_name_port() const { return host_.is_name(); }
    const std::string &name() const { return host_.name(); }

    std::string to_string() const;
    static std::optional<HostPort> from_string(std::string_view string);

private:
    friend bool operator==(const HostPort &a, const HostPort &b);
    friend bool operator!=(const HostPort &a, const HostPort &b);

    template <typename H>
    friend H AbslHashValue(H h, const HostPort &host_port);

    Host host_;
    uint16_t port_ = 0;
};

template <typename H>
H AbslHashValue(H h, const HostPort &host_port) {
    return H::combine(std::move(h), host_port.host_, host_port.port_);
}

bool AbslParseFlag(std::string_view in, HostPort *out, std::string *error);
std::string AbslUnparseFlag(const HostPort &in);

}  // namespace net

#endif  // _NET_TYPES_HOST_PORT_H
