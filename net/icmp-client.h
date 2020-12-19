#ifndef _NET_ICMP_CLIENT_H
#define _NET_ICMP_CLIENT_H

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <system_error>
#include <vector>
#include "absl/container/flat_hash_map.h"
#include "net/asio.h"
#include "net/timer-list.h"

namespace net {

// The provided executor must be single-threaded, and all functions must be
// called in the executor thread.
class IcmpClient {
public:
    struct Options {
        size_t receive_buffer_size = 2048;
        std::chrono::nanoseconds timeout = std::chrono::seconds(2);
    };

    IcmpClient(const any_io_executor &executor, const Options &options);
    IcmpClient(const IcmpClient &) = delete;
    IcmpClient &operator=(const IcmpClient &) = delete;

    void request(
        const icmp::endpoint &endpoint,
        std::vector<uint8_t> buffer,
        std::function<void(std::error_code, std::vector<uint8_t>)> callback);

private:
    class Operation;

    void receive();

    Options options_;
    icmp::socket socket_;
    TimerList timer_;
    uint_fast16_t identifier_;
    uint_fast16_t next_sequence_number_ = 0;
    std::unique_ptr<uint8_t[]> receive_buffer_;
    absl::flat_hash_map<uint16_t, Operation *> operations_;
};

}  // namespace net

#endif  // _NET_ICMP_CLIENT_H
