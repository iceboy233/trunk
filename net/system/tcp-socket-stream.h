#include "net/asio.h"
#include "net/types/stream.h"

namespace net::system {

class TcpSocketStream : public Stream {
public:
    explicit TcpSocketStream(const any_io_executor &executor);

    awaitable<std::tuple<std::error_code, size_t>> read(
        std::span<mutable_buffer const> buffers) override;

    awaitable<std::tuple<std::error_code, size_t>> write(
        std::span<const_buffer const> buffers) override;

    void close() override;

    tcp::socket &socket() { return socket_; }
    const tcp::socket &socket() const { return socket_; }

private:
    tcp::socket socket_;
};

}  // namespace net::system
