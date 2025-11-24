#ifndef _NET_TYPES_CONNECTOR_H
#define _NET_TYPES_CONNECTOR_H

#include <memory>
#include <system_error>
#include <tuple>

#include "net/asio.h"
#include "net/types/datagram.h"
#include "net/types/stream.h"

namespace net {

class Connector {
public:
    virtual ~Connector() = default;

    virtual awaitable<std::tuple<std::error_code, std::unique_ptr<Stream>>>
    connect(const tcp::endpoint &endpoint, const_buffer initial_data) = 0;

    virtual awaitable<std::tuple<std::error_code, std::unique_ptr<Datagram>>>
    bind(const udp::endpoint &endpoint) = 0;
};

}  // namespace net

#endif  // _NET_TYPES_CONNECTOR_H
