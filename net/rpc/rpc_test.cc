#include <system_error>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "net/asio.h"
#include "net/blocking-result.h"
#include "net/rpc/client.h"
#include "net/rpc/flatbuffers-request.h"
#include "net/rpc/server.h"
#include "net/rpc/flatbuffers/list_generated.h"

namespace net {
namespace rpc {
namespace {

using ::testing::ElementsAre;

TEST(RpcTest, Ping) {
    io_context io_context;
    Server server(io_context.get_executor(), {address_v4::loopback(), 0}, {});
    server.start();
    Client client(io_context.get_executor(), {});
    BlockingResult<std::error_code, std::vector<uint8_t>> result;
    client.request(server.endpoint(), "ping", {1, 2, 3}, {}, result.callback());
    ASSERT_TRUE(result.run(io_context));
    ASSERT_FALSE(std::get<0>(result.args()));
    EXPECT_THAT(std::get<1>(result.args()), ElementsAre(1, 2, 3));
}

TEST(RpcTest, List) {
    io_context io_context;
    Server server(io_context.get_executor(), {address_v4::loopback(), 0}, {});
    server.start();
    Client client(io_context.get_executor(), {});
    BlockingResult<std::error_code, fbs::ListResponseT> result;
    request<fbs::ListRequest, fbs::ListResponse>(
        client, server.endpoint(), "list", {}, {}, result.callback());
    ASSERT_TRUE(result.run(io_context));
    ASSERT_FALSE(std::get<0>(result.args()));
    EXPECT_THAT(std::get<1>(result.args()).methods,
                ElementsAre("list", "ping"));
}

}  // namespace
}  // namespace rpc
}  // namespace net
