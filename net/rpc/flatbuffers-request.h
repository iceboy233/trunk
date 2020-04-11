#ifndef _NET_RPC_FLATBUFFERS_REQUEST_H
#define _NET_RPC_FLATBUFFERS_REQUEST_H

#include <cstdint>
#include <functional>
#include <string_view>
#include <system_error>
#include <vector>
#include "net/asio.h"
#include "net/rpc/client.h"
#include "util/flatbuffers.h"

namespace net {
namespace rpc {

template <typename RequestT, typename ResponseT>
void request(
    Client &client,
    const udp::endpoint &endpoint,
    std::string_view method,
    const typename RequestT::NativeTableType &request,
    const Client::RequestOptions &options,
    std::function<void(
        std::error_code, typename ResponseT::NativeTableType)> callback) {
    std::vector<uint8_t> request_buffer;
    util::pack<RequestT>(request, request_buffer);
    client.request(
        endpoint,
        method,
        std::move(request_buffer),
        options,
        [callback = std::move(callback)](
            std::error_code ec, std::vector<uint8_t> response) {
            if (ec) {
                callback(ec, {});
                return;
            }
            typename ResponseT::NativeTableType response_object;
            if (!util::verify_and_unpack<ResponseT>(
                response, response_object)) {
                callback(make_error_code(std::errc::invalid_argument), {});
                return;
            }
            callback({}, std::move(response_object));
        });
}

}  // namespace rpc
}  // namespace net

#endif  // _NET_RPC_FLATBUFFERS_REQUEST_H
