#ifndef _NET_INTERFACE_DATAGRAM_H
#define _NET_INTERFACE_DATAGRAM_H

#include <cstddef>
#include <system_error>

#include "absl/functional/any_invocable.h"
#include "absl/types/span.h"
#include "net/asio.h"

namespace net {

class Datagram {
public:
    virtual ~Datagram() = default;

    virtual void receive_from(
        absl::Span<mutable_buffer const> buffers,
        udp::endpoint &endpoint,
        absl::AnyInvocable<void(std::error_code, size_t) &&> callback) = 0;

    virtual void send_to(
        absl::Span<const_buffer const> buffers,
        const udp::endpoint &endpoint,
        absl::AnyInvocable<void(std::error_code, size_t) &&> callback) = 0;

    virtual void close() = 0;
};

}  // namespace net

#endif  // _NET_INTERFACE_DATAGRAM_H
