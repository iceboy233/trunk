#include "net/http/client.h"

#include <optional>
#include <utility>
#include <boost/beast/core.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include "absl/strings/str_cat.h"

namespace net {
namespace http {
namespace {

struct RequestState {
    Request request;
    std::function<void(ResponseParser &)> progress_callback;
    std::function<void(std::error_code, Response)> callback;
};

}  // namespace

class Client::Connection : public boost::intrusive_ref_counter<
    Connection, boost::thread_unsafe_counter> {
public:
    virtual ~Connection() = default;
    virtual void resolve() = 0;
    virtual void write() = 0;
    virtual void close() = 0;

    void start(RequestState request_state);

    TimerList::Handle &timer_handle() { return timer_handle_; }

    std::optional<std::list<Connection *>::iterator> &connections_iter() {
        return connections_iter_;
    }

protected:
    void finish(std::error_code ec, Response response);

    RequestState request_state_;
    TimerList::Handle timer_handle_;
    std::optional<std::list<Connection *>::iterator> connections_iter_;
};

template <typename StreamT>
class Client::ConnectionImpl : public Client::Connection {
protected:
    template <typename ...ArgsT>
    ConnectionImpl(
        Client &client,
        Protocol protocol,
        std::string_view host,
        uint16_t port,
        ArgsT &&...args)
        : client_(client),
          stream_(client_.executor_, std::forward<ArgsT>(args)...),
          protocol_(protocol),
          host_(host),
          port_(port) {}

    void resolve() override;
    void connect(const tcp::resolver::results_type &endpoints);
    virtual void handshake();
    void keep_alive();
    void read_header();
    void read_some();
    void write() override;
    void close() override;

    Client &client_;
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

protected:
    void resolve() override;
    void handshake() override;
};

Client::Client(const any_io_executor &executor, const Options &options)
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
        connection->start({std::move(request),
                           std::move(options.progress_callback),
                           std::move(callback)});
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
        connection->start({std::move(request),
                           std::move(options.progress_callback),
                           std::move(callback)});
        connection->timer_handle() = timer_.schedule([this, connection]() {
            connection->timer_handle() = timer_.null_handle();
            connection->close();
        });
        connection->resolve();
    }
}

void Client::Connection::start(RequestState request_state) {
    request_state_ = std::move(request_state);
}

void Client::Connection::finish(
    std::error_code ec, Response response) {
    if (request_state_.callback) {
        request_state_.callback(ec, std::move(response));
    }
    request_state_ = {};
}

template <typename StreamT>
void Client::ConnectionImpl<StreamT>::resolve() {
    client_.tcp_resolver_.async_resolve(
        host_,
        absl::StrCat(port_),
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
void Client::ConnectionImpl<StreamT>::keep_alive() {
    if (timer_handle_ != client_.timer_.null_handle()) {
        client_.timer_.update(timer_handle_);
    }
}

template <typename StreamT>
void Client::ConnectionImpl<StreamT>::read_header() {
    response_parser_.emplace();
    async_read_header(
        stream_, buffer_, *response_parser_,
        [this, _ = boost::intrusive_ptr<ConnectionImpl<StreamT>>(this)](
            std::error_code ec, size_t) {
            if (!request_state_.callback) {
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
                if (request_state_.progress_callback) {
                    request_state_.progress_callback(*response_parser_);
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
        stream_, request_state_.request,
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
