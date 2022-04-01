#include "net/rate-limiter.h"

#include <algorithm>
#include <system_error>
#include <utility>

namespace net {
namespace {

std::chrono::nanoseconds to_duration(uint64_t size, uint64_t rate) {
    return std::chrono::nanoseconds(std::chrono::seconds(size)) / rate;
}

}  // namespace

RateLimiter::RateLimiter(
    const any_io_executor &executor,
    uint64_t rate,
    uint64_t capacity,
    uint64_t initial_size)
    : timer_(executor),
      rate_(rate),
      capacity_(to_duration(capacity, rate_)),
      size_(to_duration(initial_size, rate_)),
      update_time_(std::chrono::steady_clock::now()) {}

void RateLimiter::acquire(uint64_t size, std::function<void()> callback) {
    queue_.push({to_duration(size, rate_), std::move(callback)});
    if (queue_.size() == 1) {
        refill(std::chrono::steady_clock::now());
    }
}

bool RateLimiter::acquire_nowait(uint64_t size) {
    auto time = std::chrono::steady_clock::now();
    size_ = std::min(capacity_, size_ + (time - update_time_));
    update_time_ = time;
    std::chrono::nanoseconds size_duration = to_duration(size, rate_);
    if (size_ < size_duration) {
        return false;
    }
    size_ -= size_duration;
    return true;
}

void RateLimiter::refill(std::chrono::steady_clock::time_point time) {
    size_ = std::min(capacity_, size_ + (time - update_time_));
    update_time_ = time;
    while (true) {
        if (queue_.front().size > size_) {
            queue_.front().size -= size_;
            size_ = std::chrono::nanoseconds::zero();
            break;
        }
        size_ -= queue_.front().size;
        queue_.front().callback();
        queue_.pop();
        if (queue_.empty()) {
            return;
        }
    }
    time += queue_.front().size;
    timer_.expires_at(time);
    timer_.async_wait([this, time](std::error_code ec) {
        if (ec) {
            // TODO(iceboy): error handling.
            return;
        }
        refill(time);
    });
}

}  // namespace net
