#ifndef _NET_RPC_CLIENT_H
#define _NET_RPC_CLIENT_H

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <system_error>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/random/random.h"
#include "net/asio.h"
#include "net/rpc/operation-key.h"
#include "net/rpc/wire-structs.h"
#include "security/key.h"
#include "security/keys/zero.key.h"

namespace net {
namespace rpc {

// The provided executor must be single-threaded, and all functions must be
// called in the executor thread.
class Client {
public:
    struct Options {
        int num_channels = 1;
        size_t send_fragment_size = 1024;
        size_t receive_buffer_size = 2048;
    };

    Client(const any_io_executor &executor, const Options &options);
    ~Client();

    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;

    struct RequestOptions {
        security::Key key = security::keys::zero;
        std::chrono::nanoseconds timeout = std::chrono::seconds(2);
        std::chrono::nanoseconds initial_retry = std::chrono::milliseconds(200);
    };

    void request(
        const udp::endpoint &endpoint,
        std::string_view method,
        std::vector<uint8_t> request,
        const RequestOptions &options,
        std::function<void(std::error_code, std::vector<uint8_t>)> callback);

private:
    class Operation;

    struct Channel {
        udp::socket socket;
        std::unique_ptr<uint8_t[]> receive_buffer;
        size_t receive_size;
    };

    void receive(Channel &channel);
    void dispatch(Channel &channel);

    any_io_executor executor_;
    Options options_;
    std::vector<Channel> channels_;
    std::vector<Channel>::iterator channels_iter_;
    absl::BitGen gen_;
    absl::flat_hash_map<OperationKey, Operation *> operations_;
};

}  // namespace rpc
}  // namespace net

#endif  // _NET_RPC_CLIENT_H
