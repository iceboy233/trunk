#include "net/timer-list.h"

#include <gtest/gtest.h>
#include "net/asio.h"

namespace net {
namespace {

TEST(TimerListTest, hello) {
    io_context io_context;
    TimerList timer_list(io_context.get_executor(), std::chrono::seconds(1));
    TimerList::Handle handle = timer_list.schedule([]() {});
    timer_list.update(handle);
    timer_list.cancel(handle);
}

}  // namespace
}  // namespace net
