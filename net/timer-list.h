#ifndef _NET_TIMER_LIST_H
#define _NET_TIMER_LIST_H

#include <functional>
#include <list>
#include <system_error>
#include <utility>

#include "net/asio.h"

namespace net {

// A timer list is a timer and a list to support scheduling multiple work items
// with the same duration.
template <typename WaitableTimerT>
class BasicTimerList {
public:
    using Executor = typename WaitableTimerT::executor_type;
    using Clock = typename WaitableTimerT::clock_type;
    using Duration = typename WaitableTimerT::duration;
    using TimePoint = typename WaitableTimerT::time_point;

    explicit BasicTimerList(const Executor &executor, const Duration &duration);

    class Timer;

    awaitable<std::error_code> run(
        awaitable<std::error_code> awaitable,
        cancellation_type cancellation_type = cancellation_type::terminal);

private:
    struct Entry {
        TimePoint expiry;
        std::function<void()> callback;
    };

    using Handle = typename std::list<Entry>::iterator;

    Handle schedule(std::function<void()> callback);
    void update(Handle handle);
    void cancel(Handle handle);
    void wait();

    Duration duration_;
    WaitableTimerT timer_;
    std::list<Entry> list_;
};

using TimerList = BasicTimerList<steady_timer>;

template <typename WaitableTimerT>
BasicTimerList<WaitableTimerT>::BasicTimerList(const Executor &executor,
                                               const Duration &duration)
    : duration_(duration), timer_(executor) {}

template <typename WaitableTimerT>
typename BasicTimerList<WaitableTimerT>::Handle
BasicTimerList<WaitableTimerT>::schedule(std::function<void()> callback) {
    list_.push_back({Clock::now() + duration_, std::move(callback)});
    Handle handle = list_.end();
    --handle;
    if (list_.size() == 1) {
        wait();
    }
    return handle;
}

template <typename WaitableTimerT>
void BasicTimerList<WaitableTimerT>::update(Handle handle) {
    handle->expiry = Clock::now() + duration_;
    list_.splice(list_.end(), list_, handle);
}

template <typename WaitableTimerT>
void BasicTimerList<WaitableTimerT>::cancel(Handle handle) {
    list_.erase(handle);
    if (list_.empty()) {
        timer_.cancel();
    }
}

template <typename WaitableTimerT>
awaitable<std::error_code> BasicTimerList<WaitableTimerT>::run(
    awaitable<std::error_code> awaitable, cancellation_type cancellation_type) {
    cancellation_signal cancellation_signal;
    bool timed_out = false;
    Timer timer(*this, [cancellation_type, &cancellation_signal, &timed_out]() {
        cancellation_signal.emit(cancellation_type);
        timed_out = true;
    });
    auto result = co_await co_spawn(
        co_await this_coro::executor, std::move(awaitable),
        bind_cancellation_slot(cancellation_signal.slot(), deferred));
    if (timed_out) {
        co_return make_error_code(std::errc::timed_out);
    }
    co_return result;
}

template <typename WaitableTimerT>
void BasicTimerList<WaitableTimerT>::wait() {
    TimePoint expiry = list_.front().expiry;
    timer_.expires_at(expiry);
    timer_.async_wait([this, expiry](std::error_code ec) {
        if (ec) {
            // TODO(iceboy): Error handling.
            return;
        }
        while (!list_.empty() && list_.front().expiry <= expiry) {
            list_.front().callback();
            list_.pop_front();
        }
        if (!list_.empty()) {
            wait();
        }
    });
}

template <typename WaitableTimerT>
class BasicTimerList<WaitableTimerT>::Timer {
public:
    explicit Timer(BasicTimerList<WaitableTimerT> &list,
                   std::function<void()> callback);
    ~Timer();

    void update();

private:
    BasicTimerList<WaitableTimerT> &list_;
    std::function<void()> callback_;
    BasicTimerList<WaitableTimerT>::Handle handle_;
};

template <typename WaitableTimerT>
BasicTimerList<WaitableTimerT>::Timer::Timer(
    BasicTimerList<WaitableTimerT> &list, std::function<void()> callback)
    : list_(list),
      callback_(std::move(callback)),
      handle_(list_.schedule([this]() {
          handle_ = list_.list_.end();
          callback_();
      })) {}

template <typename WaitableTimerT>
BasicTimerList<WaitableTimerT>::Timer::~Timer() {
    if (handle_ != list_.list_.end()) {
        list_.cancel(handle_);
    }
}

template <typename WaitableTimerT>
void BasicTimerList<WaitableTimerT>::Timer::update() {
    if (handle_ == list_.list_.end()) {
        return;
    }
    list_.update(handle_);
}

}  // namespace net

#endif  // _NET_TIMER_LIST_H
