#include "net/asio.h"
#include "net/types/datagram.h"

namespace net::system {

class UdpSocketDatagram : public Datagram {
public:
    explicit UdpSocketDatagram(const any_io_executor &executor);

    awaitable<std::tuple<std::error_code, size_t>> receive_from(
        std::span<mutable_buffer const> buffers,
        udp::endpoint &endpoint) override;

    awaitable<std::tuple<std::error_code, size_t>> send_to(
        std::span<const_buffer const> buffers,
        const udp::endpoint &endpoint) override;

    void close() override;

    udp::socket &socket() { return socket_; }
    const udp::socket &socket() const { return socket_; }

private:
    udp::socket socket_;
};

}  // namespace net::system
