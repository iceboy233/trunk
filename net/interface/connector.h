#ifndef _NET_INTERFACE_CONNECTOR_H
#define _NET_INTERFACE_CONNECTOR_H

#include <cstdint>
#include <memory>
#include <string_view>
#include <system_error>

#include "absl/functional/any_invocable.h"
#include "net/asio.h"
#include "net/interface/datagram.h"
#include "net/interface/stream.h"

namespace net {

class Connector {
public:
    virtual ~Connector() = default;

    virtual void connect(
        const tcp::endpoint &endpoint,
        const_buffer initial_data,
        absl::AnyInvocable<void(
            std::error_code, std::unique_ptr<Stream>) &&> callback) = 0;

    virtual void connect(
        std::string_view host,
        uint16_t port,
        const_buffer initial_data,
        absl::AnyInvocable<void(
            std::error_code, std::unique_ptr<Stream>) &&> callback) = 0;

    virtual std::error_code bind(
        const udp::endpoint &endpoint,
        std::unique_ptr<Datagram> &datagram) = 0;
};

}  // namespace net

#endif  // _NET_INTERFACE_CONNECTOR_H
