#include "net/system/udp-socket-datagram.h"

namespace net::system {

UdpSocketDatagram::UdpSocketDatagram(const any_io_executor &executor)
    : socket_(executor) {}

awaitable<std::tuple<std::error_code, size_t>> UdpSocketDatagram::receive_from(
    std::span<mutable_buffer const> buffers, udp::endpoint &endpoint) {
    co_return co_await socket_.async_receive_from(buffers, endpoint, as_tuple);
}

awaitable<std::tuple<std::error_code, size_t>> UdpSocketDatagram::send_to(
    std::span<const_buffer const> buffers, const udp::endpoint &endpoint) {
    co_return co_await socket_.async_send_to(buffers, endpoint, as_tuple);
}

void UdpSocketDatagram::close() {
    boost::system::error_code ec;
    socket_.close(ec);
}

}  // namespace net::system
