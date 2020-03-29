#ifndef _NET_IP_ENCODING_H
#define _NET_IP_ENCODING_H

#include <cstdint>
#include <vector>
#include "absl/types/span.h"
#include "net/asio.h"

namespace net {

void encode(const address &address, std::vector<uint8_t> &bytes);
void encode(const address_v4 &address, std::vector<uint8_t> &bytes);
void encode(const address_v6 &address, std::vector<uint8_t> &bytes);
bool decode(absl::Span<const uint8_t> bytes, address &address);
bool decode(absl::Span<const uint8_t> bytes, address_v4 &address);
bool decode(absl::Span<const uint8_t> bytes, address_v6 &address);

}  // namespace net

#endif  // _NET_IP_ENCODING_H
