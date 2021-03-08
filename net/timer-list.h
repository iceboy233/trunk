#ifndef _NET_TIMER_LIST_H
#define _NET_TIMER_LIST_H

#include <functional>
#include <list>
#include <system_error>

#include "absl/container/inlined_vector.h"
#include "net/asio.h"

namespace net {

// A timer list is a timer and a list to support scheduling multiple work items
// with the same duration.
//
// The provided executor must be single-threaded, and all functions must be
// called in the executor thread.
template <
    typename ClockT,
    typename WaitTraitsT = wait_traits<ClockT>,
    typename ExecutorT = any_io_executor>
class BasicTimerList {
public:
    using Duration = typename ClockT::duration;
    using TimePoint = typename ClockT::time_point;

    BasicTimerList(const ExecutorT &executor, const Duration &duration);

    class Timer;

private:
    struct Entry;

public:
    // The handle interface is deprecated and should only be used privately.
    // Please use the timer interface instead.
    //
    // TODO(iceboy): Make this section private.
    using Handle = typename std::list<Entry>::iterator;

    // The callback will be executed in the provided executor. After the
    // callback is executed, the handle is invalidated.
    Handle schedule(std::function<void()> callback);
    void update(Handle handle);
    void cancel(Handle handle);
    Handle null_handle() { return list_.end(); }

private:
    using WaitableTimer = basic_waitable_timer<ClockT, WaitTraitsT, ExecutorT>;

    struct Entry {
        TimePoint expiry;
        std::function<void()> callback;
    };

    void wait();

    Duration duration_;
    WaitableTimer timer_;
    std::list<Entry> list_;
};

using TimerList = BasicTimerList<std::chrono::steady_clock>;

template <typename ClockT, typename WaitTraitsT, typename ExecutorT>
BasicTimerList<ClockT, WaitTraitsT, ExecutorT>::BasicTimerList(
    const ExecutorT &executor, const Duration &duration)
    : duration_(duration), timer_(executor) {}

template <typename ClockT, typename WaitTraitsT, typename ExecutorT>
typename BasicTimerList<ClockT, WaitTraitsT, ExecutorT>::Handle
BasicTimerList<ClockT, WaitTraitsT, ExecutorT>::schedule(
    std::function<void()> callback) {
    list_.push_back({ClockT::now() + duration_, std::move(callback)});
    Handle handle = list_.end();
    --handle;
    if (list_.size() == 1) {
        wait();
    }
    return handle;
}

template <typename ClockT, typename WaitTraitsT, typename ExecutorT>
void BasicTimerList<ClockT, WaitTraitsT, ExecutorT>::update(Handle handle) {
    handle->expiry = ClockT::now() + duration_;
    list_.splice(list_.end(), list_, handle);
}

template <typename ClockT, typename WaitTraitsT, typename ExecutorT>
void BasicTimerList<ClockT, WaitTraitsT, ExecutorT>::cancel(Handle handle) {
    list_.erase(handle);
    if (list_.empty()) {
        timer_.cancel();
    }
}

template <typename ClockT, typename WaitTraitsT, typename ExecutorT>
void BasicTimerList<ClockT, WaitTraitsT, ExecutorT>::wait() {
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

template <typename ClockT, typename WaitTraitsT, typename ExecutorT>
class BasicTimerList<ClockT, WaitTraitsT, ExecutorT>::Timer {
public:
    Timer(BasicTimerList<ClockT, WaitTraitsT, ExecutorT> &list);
    ~Timer() { cancel(); }

    void set();
    void wait(std::function<void(bool cancelled)> callback);
    void cancel();

private:
    BasicTimerList<ClockT, WaitTraitsT, ExecutorT> &list_;
    BasicTimerList<ClockT, WaitTraitsT, ExecutorT>::Handle handle_;
    absl::InlinedVector<std::function<void(bool cancelled)>, 1> callbacks_;
};

template <typename ClockT, typename WaitTraitsT, typename ExecutorT>
BasicTimerList<ClockT, WaitTraitsT, ExecutorT>::Timer::Timer(
    BasicTimerList<ClockT, WaitTraitsT, ExecutorT> &list)
    : list_(list),
      handle_(list_.null_handle()) {}

template <typename ClockT, typename WaitTraitsT, typename ExecutorT>
void BasicTimerList<ClockT, WaitTraitsT, ExecutorT>::Timer::set() {
    auto callbacks = std::move(callbacks_);
    callbacks_.clear();
    for (const auto &callback : callbacks) {
        callback(true);
    }
    if (handle_ != list_.null_handle()) {
        list_.update(handle_);
        return;
    }
    handle_ = list_.schedule([this]() {
        handle_ = list_.null_handle();
        auto callbacks = std::move(callbacks_);
        callbacks_.clear();
        for (const auto &callback : callbacks) {
            callback(false);
        }
    });
}

template <typename ClockT, typename WaitTraitsT, typename ExecutorT>
void BasicTimerList<ClockT, WaitTraitsT, ExecutorT>::Timer::wait(
    std::function<void(bool cancelled)> callback) {
    if (handle_ == list_.null_handle()) {
        callback(false);
        return;
    }
    callbacks_.push_back(std::move(callback));
}

template <typename ClockT, typename WaitTraitsT, typename ExecutorT>
void BasicTimerList<ClockT, WaitTraitsT, ExecutorT>::Timer::cancel() {
    if (handle_ != list_.null_handle()) {
        list_.cancel(handle_);
        handle_ = list_.null_handle();
    }
    auto callbacks = std::move(callbacks_);
    callbacks_.clear();
    for (const auto &callback : callbacks) {
        callback(true);
    }
}

}  // namespace net

#endif  // _NET_TIMER_LIST_H
