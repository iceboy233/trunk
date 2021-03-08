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

TEST(TimerListTest, initial) {
    io_context io_context;
    TestTimerList timer_list(
        io_context.get_executor(), std::chrono::seconds(1));
    TestTimerList::Timer timer(timer_list);
    testing::MockFunction<void(bool cancelled)> mock_function;
    EXPECT_CALL(mock_function, Call(false));
    timer.wait(mock_function.AsStdFunction());
    io_context.run();
}

TEST(TimerListTest, timeout) {
    io_context io_context;
    TestTimerList timer_list(
        io_context.get_executor(), std::chrono::seconds(1));
    TestTimerList::Timer timer(timer_list);
    timer.set();
    testing::MockFunction<void(bool cancelled)> mock_function;
    EXPECT_CALL(mock_function, Call(false));
    timer.wait(mock_function.AsStdFunction());
    util::MockClock::advance(std::chrono::seconds(1));
    io_context.run();
}

TEST(TimerListTest, immediate) {
    io_context io_context;
    TestTimerList timer_list(
        io_context.get_executor(), std::chrono::seconds(1));
    TestTimerList::Timer timer(timer_list);
    timer.set();
    util::MockClock::advance(std::chrono::seconds(1));
    testing::MockFunction<void(bool cancelled)> mock_function;
    EXPECT_CALL(mock_function, Call(false));
    timer.wait(mock_function.AsStdFunction());
    io_context.run();
}

TEST(TimerListTest, update) {
    io_context io_context;
    TestTimerList timer_list(
        io_context.get_executor(), std::chrono::seconds(2));
    TestTimerList::Timer timer(timer_list);
    timer.set();
    util::MockClock::advance(std::chrono::seconds(1));
    testing::MockFunction<void(bool cancelled)> mock_function1;
    EXPECT_CALL(mock_function1, Call(true));
    timer.wait(mock_function1.AsStdFunction());
    timer.set();
    util::MockClock::advance(std::chrono::seconds(2));
    testing::MockFunction<void(bool cancelled)> mock_function2;
    EXPECT_CALL(mock_function2, Call(false));
    timer.wait(mock_function2.AsStdFunction());
    io_context.run();
}

TEST(TimerListTest, cancel) {
    io_context io_context;
    TestTimerList timer_list(
        io_context.get_executor(), std::chrono::seconds(1));
    TestTimerList::Timer timer(timer_list);
    timer.set();
    testing::MockFunction<void(bool cancelled)> mock_function;
    EXPECT_CALL(mock_function, Call(true));
    timer.wait(mock_function.AsStdFunction());
    timer.cancel();
    io_context.run();
}

}  // namespace
}  // namespace net
