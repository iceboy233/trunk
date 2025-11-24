#include "net/asio.h"
#include "net/types/connector.h"

namespace net {

class SystemConnector : public net::Connector {
public:
    struct Options {
        bool tcp_no_delay = true;
        int tcp_fast_open_connect = 1;
    };

    explicit SystemConnector(const Options &options);

    awaitable<std::tuple<std::error_code, std::unique_ptr<Stream>>> connect(
        const tcp::endpoint &endpoint, const_buffer initial_data) override;

    awaitable<std::tuple<std::error_code, std::unique_ptr<Datagram>>> bind(
        const udp::endpoint &endpoint) override;

private:
    class TcpStream;
    class UdpDatagram;

    bool tcp_no_delay_;
    int tcp_fast_open_connect_;
};

}  // namespace net
