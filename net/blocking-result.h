#ifndef _NET_BLOCKING_RESULT_H
#define _NET_BLOCKING_RESULT_H

#include <tuple>
#include "net/asio.h"

namespace net {

template <typename ...ArgsT>
class BlockingResult : public std::tuple<ArgsT...> {
public:
    BlockingResult() = default;
    BlockingResult(const BlockingResult &) = delete;
    BlockingResult &operator=(const BlockingResult &) = delete;

    auto callback();
    bool run(io_context &io_context);
    void reset() { done_ = false; }

    template <size_t I>
    auto &get() {
        return std::get<I>(static_cast<std::tuple<ArgsT...> &>(*this));
    }

    template <size_t I>
    const auto &get() const {
        return std::get<I>(static_cast<const std::tuple<ArgsT...> &>(*this));
    }

private:
    bool done_ = false;
};

template <typename ...ArgsT>
auto BlockingResult<ArgsT...>::callback() {
    return [this](ArgsT ...args) {
        static_cast<std::tuple<ArgsT...> &>(*this) = std::make_tuple(
            std::move(args)...);
        done_ = true;
    };
}

template <typename ...ArgsT>
bool BlockingResult<ArgsT...>::run(io_context &io_context) {
    while (!done_) {
        if (!io_context.run_one()) {
            return false;
        }
    }
    return true;
}

}  // namespace net

#endif  // _NET_BLOCKING_RESULT_H
