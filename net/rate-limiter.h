#ifndef _NET_RATE_LIMITER_H
#define _NET_RATE_LIMITER_H

#include <chrono>
#include <cstdint>
#include <functional>
#include <queue>
#include "net/asio.h"

namespace net {

// The provided executor must be single-threaded, and all functions must be
// called in the executor thread.
class RateLimiter {
public:
    RateLimiter(
        executor &executor,
        uint64_t rate,
        uint64_t capacity,
        uint64_t initial_size);
    RateLimiter(const RateLimiter &) = delete;
    RateLimiter &operator=(const RateLimiter &) = delete;

    void acquire(uint64_t size, std::function<void()> callback);

private:
    void refill();

    struct WaitEntry {
        uint64_t size;
        std::function<void()> callback;
    };

    executor executor_;
    steady_timer timer_;
    const uint64_t rate_;
    const uint64_t capacity_;
    uint64_t size_;
    std::chrono::steady_clock::time_point update_time_;
    std::queue<WaitEntry> queue_;
};

}  // namespace net

#endif  // _NET_RATE_LIMITER_H
