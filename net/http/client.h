#ifndef _NET_HTTP_CLIENT_H
#define _NET_HTTP_CLIENT_H

#include <chrono>
#include <cstdint>
#include <functional>
#include <list>
#include <string>
#include <string_view>
#include <system_error>
#include <boost/asio/ssl.hpp>
#include "absl/container/flat_hash_map.h"
#include "net/asio.h"
#include "net/timer-list.h"
#include "net/http/beast.h"

namespace net {
namespace http {

// The provided executor must be single-threaded, and all functions must be
// called in the executor thread.
class Client {
public:
    struct Options {
        std::chrono::nanoseconds connection_timeout = std::chrono::seconds(60);
        boost::asio::ssl::context_base::method ssl_method =
            boost::asio::ssl::context_base::sslv23_client;
    };

    Client(const executor &executor, const Options &options);
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;

    enum class Protocol {
        http,
        https_verify_none,
    };

    struct RequestOptions {
        std::function<void(const ResponseParser &)> progress_callback = {};
    };

    void request(
        Protocol protocol,
        std::string_view host,
        uint16_t port,
        Request request,
        RequestOptions options,
        std::function<void(std::error_code, Response)> callback);

private:
    struct ConnectionKey {
        Protocol protocol;
        std::string host;
        uint16_t port;
    };

    template <typename H>
    friend H AbslHashValue(H h, const ConnectionKey &key) {
        return H::combine(std::move(h), key.protocol, key.host, key.port);
    }

    friend bool operator==(const ConnectionKey &a, const ConnectionKey &b) {
        return a.protocol == b.protocol && a.host == b.host &&
               a.port == b.port;
    }

    class Connection;

    template <typename StreamT>
    class ConnectionImpl;

    class HttpConnection;
    class HttpsVerifyNoneConnection;

    executor executor_;
    Options options_;
    tcp::resolver tcp_resolver_;
    boost::asio::ssl::context ssl_context_;
    absl::flat_hash_map<ConnectionKey, std::list<Connection *>> connections_;
    TimerList timer_;
};

}  // namespace http
}  // namespace net

#endif  // _NET_HTTP_CLIENT_H
