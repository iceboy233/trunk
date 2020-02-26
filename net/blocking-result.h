#ifndef _NET_BLOCKING_RESULT_H
#define _NET_BLOCKING_RESULT_H

#include <tuple>
#include "net/asio.h"

namespace net {

template <typename ...ArgsT>
class BlockingResult {
public:
    BlockingResult() = default;
    BlockingResult(const BlockingResult &) = delete;
    BlockingResult &operator=(const BlockingResult &) = delete;

    auto callback();
    bool run(io_context &io_context);
    void reset() { done_ = false; }

    std::tuple<ArgsT...> &args() { return args_; }
    const std::tuple<ArgsT...> &args() const { return args_; }

private:
    bool done_ = false;
    std::tuple<ArgsT...> args_;
};

template <typename ...ArgsT>
auto BlockingResult<ArgsT...>::callback() {
    return [this](ArgsT ...args) {
        done_ = true;
        args_ = std::make_tuple(std::move(args)...);
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
