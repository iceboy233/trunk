#ifndef _NET_ASIO_HASH_H
#define _NET_ASIO_HASH_H

#include <boost/asio.hpp>

namespace boost {
namespace asio {
namespace ip {

template <typename H>
H AbslHashValue(H h, const address_v4 &address) {
    return H::combine(std::move(h), address.to_uint());
}

template <typename H>
H AbslHashValue(H h, const address_v6 &address) {
    auto bytes = address.to_bytes();
    return H::combine_contiguous(std::move(h), bytes.data(), bytes.size());
}

template <typename H>
H AbslHashValue(H h, const address &address) {
    if (address.is_v4()) {
        return H::combine(std::move(h), address.to_v4());
    } else {
        return H::combine(std::move(h), address.to_v6());
    }
}

template <typename H>
H AbslHashValue(H h, const tcp::endpoint &endpoint) {
    return H::combine(std::move(h), endpoint.address(), endpoint.port());
}

template <typename H>
H AbslHashValue(H h, const udp::endpoint &endpoint) {
    return H::combine(std::move(h), endpoint.address(), endpoint.port());
}

}  // namespace ip
}  // namespace asio
}  // namespace boost

#endif  // _NET_ASIO_HASH_H
