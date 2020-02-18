#include "net/asio.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace net {
namespace {

TEST(AsioTest, post) {
    io_context io_context;
    testing::MockFunction<void()> mock_function;
    post(io_context, mock_function.AsStdFunction());
    EXPECT_CALL(mock_function, Call());
    io_context.run();
}

}  // namespace
}  // namespace net
