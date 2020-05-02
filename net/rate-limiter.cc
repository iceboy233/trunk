#include "net/rate-limiter.h"

#include <system_error>

namespace net {

RateLimiter::RateLimiter(
    executor &executor,
    uint64_t rate,
    uint64_t capacity,
    uint64_t initial_size)
    : executor_(executor),
      timer_(executor_),
      rate_(rate),
      capacity_(capacity),
      size_(initial_size),
      update_time_(std::chrono::steady_clock::now()) {}

void RateLimiter::acquire(uint64_t size, std::function<void()> callback) {
    queue_.push({size, std::move(callback)});
    if (queue_.size() == 1) {
        refill();
    }
}

void RateLimiter::refill() {
    auto now = std::chrono::steady_clock::now();
    uint64_t delta = std::chrono::duration_cast<std::chrono::seconds>(
        (now - update_time_) * rate_).count();
    size_ = std::min(capacity_, size_ + delta);
    update_time_ = now;
    while (true) {
        if (queue_.front().size > size_) {
            queue_.front().size -= size_;
            size_ = 0;
            break;
        }
        size_ -= queue_.front().size;
        queue_.front().callback();
        queue_.pop();
        if (queue_.empty()) {
            return;
        }
    }
    timer_.expires_at(
        now + std::chrono::nanoseconds(
            queue_.front().size * 1000000000 / rate_));
    timer_.async_wait([this](std::error_code ec) {
        if (ec) {
            // TODO(iceboy): error handling.
            return;
        }
        refill();
    });
}

}  // namespace net
