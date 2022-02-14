#ifndef _NET_RPC_SERVER_H
#define _NET_RPC_SERVER_H

#include <memory>
#include <string>
#include <string_view>

#include "absl/container/flat_hash_map.h"
#include "absl/types/span.h"
#include "net/asio.h"
#include "net/rpc/handler.h"
#include "net/rpc/operation-key.h"
#include "security/key.h"
#include "security/keystore.h"

namespace net {
namespace rpc {

// The provided executor must be single-threaded, and all functions must be
// called in the executor thread.
class Server {
public:
    struct Options {
        size_t send_fragment_size = 1024;
        size_t receive_buffer_size = 2048;
    };

    Server(
        const any_io_executor &executor,
        const udp::endpoint &endpoint,
        const Options &options);
    ~Server();

    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

    void handle(std::string_view name, std::unique_ptr<Handler> handler);
    void add_key(const security::Key &key);
    void start() { receive(); }

    udp::endpoint endpoint() const { return socket_.local_endpoint(); }

private:
    class PingHandler;
    class ListHandler;
    class Operation;

    void receive();
    void dispatch();

    any_io_executor executor_;
    Options options_;
    udp::socket socket_;
    absl::flat_hash_map<std::string, std::unique_ptr<Handler>> handlers_;
    security::Keystore keystore_;
    absl::flat_hash_map<OperationKey, Operation *> operations_;
    std::unique_ptr<uint8_t[]> receive_buffer_;
    udp::endpoint receive_endpoint_;
    size_t receive_size_;
};

}  // namespace rpc
}  // namespace net

#endif  // _NET_RPC_SERVER_H
