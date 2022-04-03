#include "net/rate-limiter.h"

#include <algorithm>
#include <system_error>
#include <utility>

namespace net {

RateLimiter::RateLimiter(
    const any_io_executor &executor,
    uint64_t rate,
    std::chrono::nanoseconds capacity)
    : timer_(executor),
      rate_(rate),
      capacity_(capacity) {}

void RateLimiter::acquire(uint64_t size, std::function<void()> callback) {
    queue_.push({
        std::chrono::nanoseconds(std::chrono::seconds(size)) / rate_,
        std::move(callback)});
    if (queue_.size() == 1) {
        update(std::chrono::steady_clock::now());
    }
}

bool RateLimiter::acquire_nowait(uint64_t size) {
    auto time = std::chrono::steady_clock::now();
    duration_ = std::min(duration_ + (time - time_), capacity_);
    time_ = time;
    std::chrono::nanoseconds duration =
        std::chrono::nanoseconds(std::chrono::seconds(size)) / rate_;
    if (duration_ < duration) {
        return false;
    }
    duration_ -= duration;
    return true;
}

void RateLimiter::update(std::chrono::steady_clock::time_point time) {
    duration_ = std::min(duration_ + (time - time_), capacity_);
    time_ = time;
    while (true) {
        if (queue_.front().duration > duration_) {
            queue_.front().duration -= duration_;
            duration_ = std::chrono::nanoseconds::zero();
            break;
        }
        duration_ -= queue_.front().duration;
        queue_.front().callback();
        queue_.pop();
        if (queue_.empty()) {
            return;
        }
    }
    time += std::min(queue_.front().duration, capacity_);
    timer_.expires_at(time);
    timer_.async_wait([this, time](std::error_code ec) {
        if (ec) {
            // TODO(iceboy): error handling.
            return;
        }
        update(time);
    });
}

}  // namespace net
