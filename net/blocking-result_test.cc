#include "net/blocking-result.h"

#include <gtest/gtest.h>
#include "net/asio.h"

namespace net {
namespace {

TEST(BlockingResultTest, run) {
    io_context io_context;
    BlockingResult<int> result;
    post(io_context, [callback = result.callback()]() { callback(123); });
    ASSERT_TRUE(result.run(io_context));
    EXPECT_EQ(std::get<0>(result.args()), 123);
}

}  // namespace
}  // namespace net
