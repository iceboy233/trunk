#ifndef _UTIL_HASH_FILTER_H
#define _UTIL_HASH_FILTER_H

#include <array>
#include <cstdint>
#include <memory>
#include <type_traits>

#include "absl/random/random.h"

namespace util {

// Hash filter with 32 bits per entry.
//
// The implementation follows the article:
//
// Cuckoo Filter: Practically Better Than Bloom
// http://www.cs.cmu.edu/~binfan/papers/conext14_cuckoofilter.pdf
class HashFilter32 {
public:
    explicit HashFilter32(uint32_t num_buckets);

    void clear();
    bool insert(uint64_t fingerprint);
    bool test(uint64_t fingerprint) const;

    uint32_t size() const { return size_; }

private:
    struct Bucket {
        std::array<uint32_t, 4> entries;
    };
    static_assert(std::is_trivial_v<Bucket>);

    static bool add(uint32_t fp32, Bucket &bucket);
    static bool find_two(const Bucket &b0, const Bucket &b1, uint32_t fp32);

    std::unique_ptr<Bucket[]> buckets_;
    uint32_t num_buckets_mask_;
    uint32_t size_;
    absl::InsecureBitGen gen_;
};

}  // namespace util

#endif  // _UTIL_HASH_FILTER_H
