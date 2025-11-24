#ifndef _NET_TYPES_STREAM_H
#define _NET_TYPES_STREAM_H

#include <cstddef>
#include <span>
#include <system_error>
#include <tuple>

#include "net/asio.h"

namespace net {

class Stream {
public:
    virtual ~Stream() = default;

    virtual awaitable<std::tuple<std::error_code, size_t>> read(
        std::span<mutable_buffer const> buffers) = 0;

    virtual awaitable<std::tuple<std::error_code, size_t>> write(
        std::span<const_buffer const> buffers) = 0;

    virtual void close() = 0;
};

}  // namespace net

#endif  // _NET_TYPES_STREAM_H
