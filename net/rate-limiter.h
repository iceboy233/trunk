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
        const any_io_executor &executor,
        uint64_t rate,
        std::chrono::nanoseconds capacity);

    RateLimiter(const RateLimiter &) = delete;
    RateLimiter &operator=(const RateLimiter &) = delete;

    void acquire(uint64_t size, std::function<void()> callback);
    bool acquire_nowait(uint64_t size);

private:
    void update(std::chrono::steady_clock::time_point time);

    struct WaitEntry {
        std::chrono::nanoseconds duration;
        std::function<void()> callback;
    };

    steady_timer timer_;
    const uint64_t rate_;
    const std::chrono::nanoseconds capacity_;
    std::chrono::nanoseconds duration_ = std::chrono::nanoseconds::zero();
    std::chrono::steady_clock::time_point time_;
    std::queue<WaitEntry> queue_;
};

}  // namespace net

#endif  // _NET_RATE_LIMITER_H
