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

TEST(TimerListTest, timeout) {
    io_context io_context;
    TestTimerList timer_list(
        io_context.get_executor(), std::chrono::seconds(1));
    testing::MockFunction<void()> function;
    EXPECT_CALL(function, Call());
    TestTimerList::Timer timer(timer_list, function.AsStdFunction());
    util::MockClock::advance(std::chrono::seconds(1));
    io_context.run();
}

TEST(TimerListTest, update) {
    io_context io_context;
    TestTimerList timer_list(
        io_context.get_executor(), std::chrono::seconds(2));
    testing::MockFunction<void()> function;
    EXPECT_CALL(function, Call());
    TestTimerList::Timer timer(timer_list, function.AsStdFunction());
    util::MockClock::advance(std::chrono::seconds(1));
    timer.update();
    util::MockClock::advance(std::chrono::seconds(2));
    io_context.run();
}

TEST(TimerListTest, cancel) {
    io_context io_context;
    TestTimerList timer_list(
        io_context.get_executor(), std::chrono::seconds(1));
    testing::MockFunction<void()> function;
    EXPECT_CALL(function, Call()).Times(0);
    {
        TestTimerList::Timer timer(timer_list, function.AsStdFunction());
    }
    io_context.run();
}

}  // namespace
}  // namespace net
