#include "net/http/client.h"

#include <optional>
#include <utility>
#include <boost/beast/core.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

namespace net {
namespace http {

class Client::Connection : public boost::intrusive_ref_counter<
    Connection, boost::thread_unsafe_counter> {
public:
    explicit Connection(Client &client) : client_(client) {}
    Connection(const Connection &connection) = delete;
    Connection &operator=(const Connection &connection) = delete;
    virtual ~Connection() = default;

    virtual void resolve() = 0;
    virtual void write() = 0;
    virtual void close() = 0;

    void attach(
        Request &&request,
        std::function<void(ResponseParser &)> &&progress_callback,
        std::function<void(std::error_code, Response)> &&callback);
    void keep_alive();
    void finish(std::error_code ec, Response response);

    TimerList::Handle &timer_handle() { return timer_handle_; }

    std::optional<std::list<Connection *>::iterator> &connections_iter() {
        return connections_iter_;
    }

protected:
    Client &client_;
    Request request_;
    std::function<void(ResponseParser &)> progress_callback_;
    std::function<void(std::error_code, Response)> callback_;
    TimerList::Handle timer_handle_;
    std::optional<std::list<Connection *>::iterator> connections_iter_;
};

template <typename StreamT>
class Client::ConnectionImpl : public Client::Connection {
public:
    template <typename ...ArgsT>
    ConnectionImpl(
        Client &client,
        Protocol protocol,
        std::string_view host,
        uint16_t port,
        ArgsT &&...args)
        : Connection(client),
          stream_(client_.executor_, std::forward<ArgsT>(args)...),
          protocol_(protocol),
          host_(host),
          port_(port) {}

    void resolve();
    void connect(const tcp::resolver::results_type &endpoints);
    virtual void handshake();
    void read_header();
    void read_some();
    void write() override;
    void close() override;

protected:
    StreamT stream_;
    Protocol protocol_;
    std::string host_;
    uint16_t port_;
    boost::beast::flat_buffer buffer_;
    std::optional<ResponseParser> response_parser_;
};

class Client::HttpConnection : public Client::ConnectionImpl<tcp::socket> {
public:
    HttpConnection(Client &client, std::string_view host, uint16_t port)
        : ConnectionImpl(client, Protocol::http, host, port) {}
};

class Client::HttpsVerifyNoneConnection
    : public Client::ConnectionImpl<boost::asio::ssl::stream<tcp::socket>> {
public:
    HttpsVerifyNoneConnection(
        Client &client, std::string_view host, uint16_t port)
        : ConnectionImpl(
            client, Protocol::https_verify_none, host, port,
            client.ssl_context_) {}

    void resolve() override;
    void handshake() override;
};

Client::Client(const executor &executor, const Options &options)
    : executor_(executor),
      options_(options),
      tcp_resolver_(executor_),
      ssl_context_(options_.ssl_method),
      timer_(executor_, options_.connection_timeout) {}

void Client::request(
    Protocol protocol,
    std::string_view host,
    uint16_t port,
    Request request,
    RequestOptions options,
    std::function<void(std::error_code, Response)> callback) {
    auto iter = connections_.find({protocol, std::string(host), port});
    if (iter != connections_.end()) {
        std::list<Connection *> &list = iter->second;
        boost::intrusive_ptr<Connection> connection(list.front());
        list.pop_front();
        if (list.empty()) {
            connections_.erase(iter);
        }
        connection->attach(
            std::move(request),
            std::move(options.progress_callback),
            std::move(callback));
        connection->connections_iter() = std::nullopt;
        connection->write();
    } else {
        boost::intrusive_ptr<Connection> connection;
        switch (protocol) {
        case Protocol::http:
            connection = new HttpConnection(*this, host, port);
            break;
        case Protocol::https_verify_none:
            connection = new HttpsVerifyNoneConnection(*this, host, port);
            break;
        }
        connection->attach(
            std::move(request),
            std::move(options.progress_callback),
            std::move(callback));
        connection->timer_handle() = timer_.schedule([this, connection]() {
            connection->timer_handle() = timer_.null_handle();
            connection->close();
        });
        connection->resolve();
    }
}

void Client::Connection::attach(
    Request &&request,
    std::function<void(ResponseParser &)> &&progress_callback,
    std::function<void(std::error_code, Response)> &&callback) {
    request_ = std::move(request);
    progress_callback_ = std::move(progress_callback);
    callback_ = std::move(callback);
}

void Client::Connection::keep_alive() {
    if (timer_handle_ != client_.timer_.null_handle()) {
        client_.timer_.update(timer_handle_);
    }
}

void Client::Connection::finish(
    std::error_code ec, Response response) {
    if (callback_) {
        callback_(ec, std::move(response));
    }
    request_ = {};
    progress_callback_ = {};
    callback_ = {};
}

template <typename StreamT>
void Client::ConnectionImpl<StreamT>::resolve() {
    client_.tcp_resolver_.async_resolve(
        host_,
        std::to_string(port_),
        [this, _ = boost::intrusive_ptr<ConnectionImpl<StreamT>>(this)](
            std::error_code ec, const tcp::resolver::results_type &endpoints) {
            if (ec) {
                finish(ec, {});
                return;
            }
            connect(endpoints);
        });
}

template <typename StreamT>
void Client::ConnectionImpl<StreamT>::connect(
    const tcp::resolver::results_type &endpoints) {
    async_connect(
        stream_.lowest_layer(),
        endpoints,
        [this, _ = boost::intrusive_ptr<ConnectionImpl<StreamT>>(this)](
            std::error_code ec, const tcp::endpoint &) {
            if (ec) {
                finish(ec, {});
                return;
            }
            handshake();
        });
}

template <typename StreamT>
void Client::ConnectionImpl<StreamT>::handshake() {
    read_header();
    write();
}

template <typename StreamT>
void Client::ConnectionImpl<StreamT>::read_header() {
    response_parser_.emplace();
    async_read_header(
        stream_, buffer_, *response_parser_,
        [this, _ = boost::intrusive_ptr<ConnectionImpl<StreamT>>(this)](
            std::error_code ec, size_t) {
            if (!callback_) {
                // Received unexpected header.
                close();
                return;
            }
            if (ec) {
                finish(ec, {});
                close();
                return;
            }
            keep_alive();
            read_some();
        });
}

template <typename StreamT>
void Client::ConnectionImpl<StreamT>::read_some() {
    async_read_some(
        stream_, buffer_, *response_parser_,
        [this, _ = boost::intrusive_ptr<ConnectionImpl<StreamT>>(this)](
            std::error_code ec, size_t) {
            if (ec) {
                finish(ec, {});
                close();
                return;
            }
            if (!response_parser_->is_done()) {
                if (progress_callback_) {
                    progress_callback_(*response_parser_);
                }
                keep_alive();
                read_some();
                return;
            }
            buffer_ = {};
            finish({}, response_parser_->release());
            std::list<Connection *> &list =
                client_.connections_[{protocol_, host_, port_}];
            list.push_front(this);
            connections_iter_ = list.begin();
            keep_alive();
            read_header();
        });
}

template <typename StreamT>
void Client::ConnectionImpl<StreamT>::write() {
    keep_alive();
    async_write(
        stream_, request_,
        [this, _ = boost::intrusive_ptr<ConnectionImpl<StreamT>>(this)](
            std::error_code ec, size_t) {
            if (ec) {
                finish(ec, {});
                close();
                return;
            }
        });
}

template <typename StreamT>
void Client::ConnectionImpl<StreamT>::close() {
    finish(make_error_code(std::errc::timed_out), {});
    stream_.lowest_layer().close();
    if (timer_handle_ != client_.timer_.null_handle()) {
        client_.timer_.cancel(timer_handle_);
        timer_handle_ = client_.timer_.null_handle();
    }
    if (connections_iter_) {
        auto iter = client_.connections_.find({protocol_, host_, port_});
        std::list<Connection *> &list = iter->second;
        list.erase(*connections_iter_);
        if (list.empty()) {
            client_.connections_.erase(iter);
        }
        connections_iter_ = std::nullopt;
    }
}

void Client::HttpsVerifyNoneConnection::resolve() {
    stream_.set_verify_mode(boost::asio::ssl::verify_none);
    if (!SSL_set_tlsext_host_name(stream_.native_handle(), host_.c_str())) {
        std::error_code ec(
            static_cast<int>(ERR_get_error()),
            boost::asio::error::get_ssl_category());
        finish(ec, {});
        return;
    }
    ConnectionImpl::resolve();
}

void Client::HttpsVerifyNoneConnection::handshake() {
    stream_.async_handshake(
        boost::asio::ssl::stream_base::client,
        [this, _ = boost::intrusive_ptr<HttpsVerifyNoneConnection>(this)](
            std::error_code ec) {
            if (ec) {
                finish(ec, {});
                close();
                return;
            }
            ConnectionImpl::handshake();
        });
}

}  // namespace http
}  // namespace net
