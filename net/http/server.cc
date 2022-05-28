#include "net/http/server.h"

#include <utility>
#include <boost/beast/core.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

namespace net {
namespace http {

template <typename StreamT>
class Server::Connection : public boost::intrusive_ref_counter<
    Connection<StreamT>, boost::thread_unsafe_counter> {
public:
    Connection(Server &server, StreamT &&stream);

private:
    void read();
    void handle();
    void write();
    void close();

    Server &server_;
    StreamT stream_;
    boost::beast::flat_buffer buffer_;
    Request request_;
    Response response_;
    std::optional<TimerList::Timer> timer_;
};

Server::Server(
    const any_io_executor &executor,
    const tcp::endpoint &endpoint,
    Handler handler,
    const Options &options)
    : acceptor_(executor, endpoint),
      timer_list_(executor, options.connection_timeout),
      handler_(std::move(handler)) { accept(); }

void Server::accept() {
    acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
        if (ec) {
            return;
        }
        new Connection<tcp::socket>(*this, std::move(socket));
        accept();
    });
}

template <typename StreamT>
Server::Connection<StreamT>::Connection(Server &server, StreamT &&stream)
    : server_(server),
      stream_(std::move(stream)),
      timer_(std::in_place, server_.timer_list_, [this]() { close(); }) {
    read();
}

template <typename StreamT>
void Server::Connection<StreamT>::read() {
    request_ = {};
    async_read(
        stream_, buffer_, request_,
        [connection = boost::intrusive_ptr<Connection<StreamT>>(this)](
            std::error_code ec, size_t) {
            if (ec) {
                connection->close();
                return;
            }
            connection->handle();
            connection->timer_->update();
        });
}

template <typename StreamT>
void Server::Connection<StreamT>::handle() {
    server_.handler_(
        request_, response_,
        [connection = boost::intrusive_ptr<Connection<StreamT>>(this)](
            std::error_code ec) {
            if (ec) {
                connection->close();
                return;
            }
            connection->write();
            connection->timer_->update();
        });
}

template <typename StreamT>
void Server::Connection<StreamT>::write() {
    async_write(
        stream_, response_,
        [connection = boost::intrusive_ptr<Connection<StreamT>>(this)](
            std::error_code ec, size_t) {
            if (ec || connection->response_.need_eof()) {
                connection->close();
                return;
            }
            connection->read();
            connection->timer_->update();
        });
}

template <typename StreamT>
void Server::Connection<StreamT>::close() {
    timer_.reset();
    stream_.close();
}

}  // namespace http
}  // namespace net
