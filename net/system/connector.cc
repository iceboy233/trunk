#include "net/system/connector.h"

#include "net/system/tcp-socket-stream.h"
#include "net/system/udp-socket-datagram.h"

namespace net::system {

Connector::Connector(const Options &options)
    : tcp_no_delay_(options.tcp_no_delay),
      tcp_fast_open_connect_(options.tcp_fast_open_connect) {}

awaitable<std::tuple<std::error_code, std::unique_ptr<Stream>>>
Connector::connect(const tcp::endpoint &endpoint, const_buffer initial_data) {
    auto executor = co_await this_coro::executor;
    auto stream = std::make_unique<TcpSocketStream>(executor);
    tcp::socket &socket = stream->socket();
    boost::system::error_code ec;
    socket.open(endpoint.protocol(), ec);
    if (ec) [[unlikely]] {
        co_return std::tuple(ec, nullptr);
    }
    if (tcp_no_delay_) {
        socket.set_option(tcp::no_delay(true), ec);
        if (ec) [[unlikely]] {
            co_return std::tuple(ec, nullptr);
        }
    }
#ifdef TCP_FASTOPEN_CONNECT
    switch (tcp_fast_open_connect_) {
    case 1:
        if (!initial_data.size()) {
            break;
        }
        [[fallthrough]];
    case 2:
        socket.set_option(boost::asio::detail::socket_option::boolean<
                              IPPROTO_TCP, TCP_FASTOPEN_CONNECT>(true),
                          ec);
        if (ec) [[unlikely]] {
            co_return std::tuple(ec, nullptr);
        }
    }
#endif
    auto [ec2] = co_await socket.async_connect(endpoint, as_tuple(deferred));
    if (ec2) [[unlikely]] {
        co_return std::tuple(ec2, nullptr);
    }
    if (initial_data.size()) {
        auto [ec, size] =
            co_await async_write(socket, initial_data, as_tuple(deferred));
        if (ec) [[unlikely]] {
            co_return std::tuple(ec, nullptr);
        }
    }
    co_return std::tuple(std::error_code(), std::move(stream));
}

awaitable<std::tuple<std::error_code, std::unique_ptr<Datagram>>>
Connector::bind(const udp::endpoint &endpoint) {
    auto executor = co_await this_coro::executor;
    auto datagram = std::make_unique<UdpSocketDatagram>(executor);
    udp::socket &socket = datagram->socket();
    boost::system::error_code ec;
    socket.open(endpoint.protocol(), ec);
    if (ec) [[unlikely]] {
        co_return std::tuple(ec, nullptr);
    }
    socket.bind(endpoint, ec);
    if (ec) [[unlikely]] {
        co_return std::tuple(ec, nullptr);
    }
    co_return std::tuple(std::error_code(), std::move(datagram));
}

}  // namespace net::system
