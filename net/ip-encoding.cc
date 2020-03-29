#include "net/ip-encoding.h"

#include <algorithm>

namespace net {

void encode(const address &address, std::vector<uint8_t> &bytes) {
    if (address.is_v4()) {
        encode(address.to_v4(), bytes);
    } else {
        encode(address.to_v6(), bytes);
    }
}

void encode(const address_v4 &address, std::vector<uint8_t> &bytes) {
    auto bytes_array = address.to_bytes();
    bytes.assign(bytes_array.begin(), bytes_array.end());
}

void encode(const address_v6 &address, std::vector<uint8_t> &bytes) {
    auto bytes_array = address.to_bytes();
    bytes.assign(bytes_array.begin(), bytes_array.end());
}

bool decode(absl::Span<const uint8_t> bytes, address &address) {
    if (bytes.size() == 4) {
        address_v4::bytes_type bytes_array;
        std::copy_n(bytes.begin(), 4, bytes_array.begin());
        address = address_v4(bytes_array);
        return true;
    } else if (bytes.size() == 16) {
        address_v6::bytes_type bytes_array;
        std::copy_n(bytes.begin(), 16, bytes_array.begin());
        address = address_v6(bytes_array);
        return true;
    }
    return false;
}

bool decode(absl::Span<const uint8_t> bytes, address_v4 &address) {
    if (bytes.size() != 4) {
        return false;
    }
    address_v4::bytes_type bytes_array;
    std::copy(bytes.begin(), bytes.end(), bytes_array.begin());
    address = address_v4(bytes_array);
    return true;
}

bool decode(absl::Span<const uint8_t> bytes, address_v6 &address) {
    if (bytes.size() != 16) {
        return false;
    }
    address_v6::bytes_type bytes_array;
    std::copy(bytes.begin(), bytes.end(), bytes_array.begin());
    address = address_v6(bytes_array);
    return true;
}

}  // namespace net
