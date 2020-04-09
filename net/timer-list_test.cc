#include "net/timer-list.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "net/asio.h"
#include "net/testing/timer.h"
#include "util/testing/clock.h"

namespace net {
namespace {

using TestTimerList = BasicTimerList<
    util::MockClock, PollingWaitTraits, io_context::executor_type>;

TEST(TimerListTest, schedule) {
    io_context io_context;
    TestTimerList timer_list(
        io_context.get_executor(), std::chrono::seconds(1));
    testing::MockFunction<void()> mock_function;
    timer_list.schedule(mock_function.AsStdFunction());
    util::MockClock::advance(std::chrono::seconds(1));
    EXPECT_CALL(mock_function, Call());
    io_context.run();
}

TEST(TimerListTest, update) {
    io_context io_context;
    TestTimerList timer_list(
        io_context.get_executor(), std::chrono::seconds(1));
    testing::MockFunction<void()> mock_function;
    auto handle = timer_list.schedule(mock_function.AsStdFunction());
    timer_list.update(handle);
    util::MockClock::advance(std::chrono::seconds(1));
    EXPECT_CALL(mock_function, Call());
    io_context.run();
}

TEST(TimerListTest, cancel) {
    io_context io_context;
    TestTimerList timer_list(
        io_context.get_executor(), std::chrono::seconds(1));
    testing::MockFunction<void()> mock_function;
    auto handle = timer_list.schedule(mock_function.AsStdFunction());
    timer_list.cancel(handle);
    util::MockClock::advance(std::chrono::seconds(1));
    EXPECT_CALL(mock_function, Call()).Times(0);
    io_context.run();
}

}  // namespace
}  // namespace net
