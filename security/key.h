#ifndef _SECURITY_KEY_H
#define _SECURITY_KEY_H

#include <cstdint>
#include <memory>
#include "absl/types/span.h"
#include "security/types.h"

namespace security {

class Key {
public:
    static const size_t encrypt_overhead = 16;

    explicit Key(const KeyArray &array);

    // out buffer should be encrypt_overhead bytes larger than in buffer.
    // If in and out alias then in.data() == out must hold.
    size_t encrypt(
        absl::Span<const uint8_t> in,
        const uint8_t nonce[12],
        uint8_t *out) const;

    // out buffer should be same size as in buffer.
    // If in and out alias then in.data() == out must hold.
    // On error, std::numeric_limits<size_t>::max() is returned.
    size_t decrypt(
        absl::Span<const uint8_t> in,
        const uint8_t nonce[12],
        uint8_t *out) const;

    uint64_t fingerprint() const;
    const KeyArray &array() const;

private:
    class Impl;
    std::shared_ptr<const Impl> impl_;
};

bool operator==(const Key &a, const Key &b);
bool operator!=(const Key &a, const Key &b);

}  // namespace security

#endif  // _SECURITY_KEY_H
