#include "net/system/tcp-socket-stream.h"

namespace net::system {

TcpSocketStream::TcpSocketStream(const any_io_executor &executor)
    : socket_(executor) {}

awaitable<std::tuple<std::error_code, size_t>> TcpSocketStream::read(
    std::span<mutable_buffer const> buffers) {
    co_return co_await socket_.async_read_some(buffers, as_tuple);
}

awaitable<std::tuple<std::error_code, size_t>> TcpSocketStream::write(
    std::span<const_buffer const> buffers) {
    co_return co_await socket_.async_write_some(buffers, as_tuple);
}

void TcpSocketStream::close() {
    boost::system::error_code ec;
    socket_.close(ec);
}

}  // namespace net::system
