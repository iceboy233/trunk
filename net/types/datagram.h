#ifndef _NET_TYPES_DATAGRAM_H
#define _NET_TYPES_DATAGRAM_H

#include <cstddef>
#include <span>
#include <system_error>
#include <tuple>

#include "net/asio.h"

namespace net {

class Datagram {
public:
    virtual ~Datagram() = default;

    virtual awaitable<std::tuple<std::error_code, size_t>> receive_from(
        std::span<mutable_buffer const> buffers, udp::endpoint &endpoint) = 0;

    virtual awaitable<std::tuple<std::error_code, size_t>> send_to(
        std::span<const_buffer const> buffers,
        const udp::endpoint &endpoint) = 0;

    virtual void close() = 0;
};

}  // namespace net

#endif  // _NET_TYPES_DATAGRAM_H
