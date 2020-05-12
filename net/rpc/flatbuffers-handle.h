#ifndef _NET_RPC_FLATBUFFERS_HANDLE_H
#define _NET_RPC_FLATBUFFERS_HANDLE_H

#include <functional>
#include <string_view>
#include <utility>
#include <vector>
#include "util/flatbuffers.h"

namespace net {
namespace rpc {

class Server;

template <typename RequestT, typename ResponseT, typename HandlerT>
void flatbuffers_handle(
    Server &server, std::string_view name, HandlerT &&handler) {
    server.handle(name, [handler = std::forward<HandlerT>(handler)](
        const std::vector<uint8_t> &request,
        const security::Key &key,
        std::function<void(std::vector<uint8_t>)> callback) {
        typename RequestT::NativeTableType request_object;
        if (!util::verify_and_unpack<RequestT>(request, request_object)) {
            return;
        }
        handler(
            std::move(request_object),
            key,
            [callback = std::move(callback)](
                const typename ResponseT::NativeTableType &response) {
                std::vector<uint8_t> response_buffer;
                util::pack<ResponseT>(response, response_buffer);
                callback(std::move(response_buffer));
            });
    });
}

}  // namespace rpc
}  // namespace net

#endif  // _NET_RPC_FLATBUFFERS_HANDLE_H
