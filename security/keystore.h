#ifndef _SECURITY_KEYSTORE_H
#define _SECURITY_KEYSTORE_H

#include <cstdint>
#include <vector>
#include "absl/container/flat_hash_map.h"
#include "security/key.h"

namespace security {

class Keystore {
public:
    void add(const Key &key);
    void find(uint64_t fingerprint, std::vector<Key> &keys) const;

private:
    absl::flat_hash_map<uint64_t, std::vector<Key>> keys_;
};

}  // namespace security

#endif  // _SECURITY_KEYSTORE_H
