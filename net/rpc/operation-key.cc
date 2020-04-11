#include "net/rpc/operation-key.h"

namespace net {
namespace rpc {

bool operator==(const OperationKey &a, const OperationKey &b) {
    return a.key_fingerprint == b.key_fingerprint &&
           a.request_id == b.request_id;
}

}  // namespace rpc
}  // namespace net
