#ifndef _NET_RPC_OPERATION_KEY_H
#define _NET_RPC_OPERATION_KEY_H

#include <cstdint>
#include <utility>

namespace net {
namespace rpc {

struct OperationKey {
    uint64_t key_fingerprint;
    uint64_t request_id;
};

template <typename H>
H AbslHashValue(H h, const OperationKey &key) {
    return H::combine(std::move(h), key.key_fingerprint, key.request_id);
}

bool operator==(const OperationKey &a, const OperationKey &b);

}  // namespace rpc
}  // namespace net

#endif  // _NET_RPC_OPERATION_KEY_H
