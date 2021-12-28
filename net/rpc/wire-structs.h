#ifndef _NET_RPC_WIRE_STRUCTS_H
#define _NET_RPC_WIRE_STRUCTS_H

#include <cstdint>

#include "base/packed.h"

namespace net {
namespace rpc {
namespace wire {
namespace request {

PACKED_BEGIN
struct PACKED Header {
    uint64_t key_fingerprint;
    uint64_t request_id;
    uint16_t offset;
    uint16_t unused;
    uint32_t flags;
};
PACKED_END
static_assert(sizeof(Header) == 24, "");

namespace flags {

constexpr uint32_t partial = 0x1;

}  // namespace flags
}  // namespace request

namespace response {

PACKED_BEGIN
struct PACKED Header {
    uint64_t key_fingerprint;
    uint64_t request_id;
    uint16_t offset;
    uint16_t unused;
    uint32_t flags;
};
PACKED_END
static_assert(sizeof(Header) == 24, "");

namespace flags {

constexpr uint32_t partial = 0x1;

}  // namespace flags
}  // namespace response
}  // namespace wire
}  // namespace rpc
}  // namespace net

#endif  // _NET_RPC_WIRE_STRUCTS_H
