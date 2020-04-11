#include "security/keystore.h"

#include <algorithm>

namespace security {

void Keystore::add(const Key &key) {
    std::vector<Key> &vector = keys_[key.fingerprint()];
    if (std::find(vector.begin(), vector.end(), key) != vector.end()) {
        return;
    }
    vector.push_back(key);
}

void Keystore::find(uint64_t fingerprint, std::vector<Key> &keys) const {
    auto iter = keys_.find(fingerprint);
    if (iter == keys_.end()) {
        keys.clear();
        return;
    }
    keys = iter->second;
}

}  // namespace security
