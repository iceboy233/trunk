#ifndef _NET_RPC_HANDLER_H
#define _NET_RPC_HANDLER_H

#include <vector>
#include "security/key.h"

namespace net {
namespace rpc {

class Handler {
public:
    virtual ~Handler() = default;
    virtual void handle(
        std::vector<uint8_t> request,
        const security::Key &key,
        std::function<void(std::vector<uint8_t>)> callback) = 0;
};

}  // namespace rpc
}  // namespace net

#endif  // _NET_RPC_HANDLER_H
