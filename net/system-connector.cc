#include "net/system-connector.h"

#include "net/types/datagram.h"
#include "net/types/stream.h"

namespace net {

using DefaultToken = as_tuple_t<deferred_t>;
using TcpSocket = DefaultToken::as_default_on_t<tcp::socket>;
using UdpSocket = DefaultToken::as_default_on_t<udp::socket>;

class SystemConnector::TcpStream : public Stream {
public:
    explicit TcpStream(const any_io_executor &executor);

    awaitable<std::tuple<std::error_code, size_t>> read(
        std::span<mutable_buffer const> buffers) override;

    awaitable<std::tuple<std::error_code, size_t>> write(
        std::span<const_buffer const> buffers) override;

    void close() override;

    TcpSocket &socket() { return socket_; }
    const TcpSocket &socket() const { return socket_; }

private:
    TcpSocket socket_;
};

class SystemConnector::UdpDatagram : public Datagram {
public:
    explicit UdpDatagram(const any_io_executor &executor);

    awaitable<std::tuple<std::error_code, size_t>> receive_from(
        std::span<mutable_buffer const> buffers,
        udp::endpoint &endpoint) override;

    awaitable<std::tuple<std::error_code, size_t>> send_to(
        std::span<const_buffer const> buffers,
        const udp::endpoint &endpoint) override;

    void close() override;

    UdpSocket &socket() { return socket_; }
    const UdpSocket &socket() const { return socket_; }

private:
    UdpSocket socket_;
};

SystemConnector::SystemConnector(const Options &options)
    : tcp_no_delay_(options.tcp_no_delay),
      tcp_fast_open_connect_(options.tcp_fast_open_connect) {}

awaitable<std::tuple<std::error_code, std::unique_ptr<Stream>>>
SystemConnector::connect(const tcp::endpoint &endpoint,
                         const_buffer initial_data) {
    auto executor = co_await this_coro::executor;
    auto stream = std::make_unique<TcpStream>(executor);
    TcpSocket &socket = stream->socket();
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
    auto [ec2] = co_await socket.async_connect(endpoint);
    if (ec2) [[unlikely]] {
        co_return std::tuple(ec2, nullptr);
    }
    if (initial_data.size()) {
        auto [ec, size] = co_await async_write(socket, initial_data);
        if (ec) [[unlikely]] {
            co_return std::tuple(ec, nullptr);
        }
    }
    co_return std::tuple(std::error_code(), std::move(stream));
}

awaitable<std::tuple<std::error_code, std::unique_ptr<Datagram>>>
SystemConnector::bind(const udp::endpoint &endpoint) {
    auto executor = co_await this_coro::executor;
    auto datagram = std::make_unique<UdpDatagram>(executor);
    UdpSocket &socket = datagram->socket();
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

SystemConnector::TcpStream::TcpStream(const any_io_executor &executor)
    : socket_(executor) {}

awaitable<std::tuple<std::error_code, size_t>> SystemConnector::TcpStream::read(
    std::span<mutable_buffer const> buffers) {
    co_return co_await socket_.async_read_some(buffers);
}

awaitable<std::tuple<std::error_code, size_t>>
SystemConnector::TcpStream::write(std::span<const_buffer const> buffers) {
    co_return co_await socket_.async_write_some(buffers);
}

void SystemConnector::TcpStream::close() {
    boost::system::error_code ec;
    socket_.close(ec);
}

SystemConnector::UdpDatagram::UdpDatagram(const any_io_executor &executor)
    : socket_(executor) {}

awaitable<std::tuple<std::error_code, size_t>>
SystemConnector::UdpDatagram::receive_from(
    std::span<mutable_buffer const> buffers, udp::endpoint &endpoint) {
    co_return co_await socket_.async_receive_from(buffers, endpoint);
}

awaitable<std::tuple<std::error_code, size_t>>
SystemConnector::UdpDatagram::send_to(std::span<const_buffer const> buffers,
                                      const udp::endpoint &endpoint) {
    co_return co_await socket_.async_send_to(buffers, endpoint);
}

void SystemConnector::UdpDatagram::close() {
    boost::system::error_code ec;
    socket_.close(ec);
}

}  // namespace net
