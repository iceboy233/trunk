#ifndef _NET_RPC_FLATBUFFERS_HANDLER_H
#define _NET_RPC_FLATBUFFERS_HANDLER_H

#include <cstdint>
#include <functional>
#include <utility>
#include <vector>
#include "net/rpc/handler.h"
#include "security/key.h"
#include "util/flatbuffers.h"

namespace net {
namespace rpc {

template <typename DerivedT, typename RequestT, typename ResponseT>
class FlatbuffersHandler : public Handler {
private:
    void handle(
        std::vector<uint8_t> request,
        const security::Key &key,
        std::function<void(std::vector<uint8_t>)> callback) final;
};

template <typename DerivedT, typename RequestT, typename ResponseT>
void FlatbuffersHandler<DerivedT, RequestT, ResponseT>::handle(
    std::vector<uint8_t> request,
    const security::Key &key,
    std::function<void(std::vector<uint8_t>)> callback) {
    typename RequestT::NativeTableType request_object;
    if (!util::verify_and_unpack<RequestT>(request, request_object)) {
        return;
    }
    static_cast<DerivedT *>(this)->handle(
        std::move(request_object),
        key,
        [callback = std::move(callback)](
            const typename ResponseT::NativeTableType &response) {
            std::vector<uint8_t> response_buffer;
            util::pack<ResponseT>(response, response_buffer);
            callback(std::move(response_buffer));
        });
}

}  // namespace rpc
}  // namespace net

#endif  // _NET_RPC_FLATBUFFERS_HANDLER_H
