#include "util/hash-filter.h"

#if defined(__AVX2__)
#include <immintrin.h>
#elif defined(__SSE2__)
#include <emmintrin.h>
#elif defined(__ARM_NEON)
#include <arm_neon.h>
#endif
#include <algorithm>
#include <cassert>
#include <cstring>

#include "absl/base/optimization.h"

namespace util {

HashFilter32::HashFilter32(uint32_t num_buckets)
    : buckets_(std::make_unique<Bucket[]>(num_buckets)),
      num_buckets_mask_(num_buckets - 1) {
    assert(!((num_buckets_mask_ + 1) & num_buckets_mask_));
    clear();
}

void HashFilter32::clear() {
    memset(buckets_.get(), 0, (num_buckets_mask_ + 1) * sizeof(Bucket));
    size_ = 0;
}

bool HashFilter32::insert(uint64_t fingerprint) {
    uint32_t fp32 = fingerprint;
    if (ABSL_PREDICT_FALSE(!fp32)) {
        fp32 = (fingerprint >> 32) | 1;
    }
    uint32_t index = (fingerprint >> 32) & num_buckets_mask_;
    if (ABSL_PREDICT_TRUE(add(fp32, buckets_[index]))) {
        ++size_;
        return true;
    }
    index ^= fp32 & num_buckets_mask_;
    if (add(fp32, buckets_[index])) {
        ++size_;
        return true;
    }
    for (int i = 0; i < 16; ++i) {
        uint64_t seed = absl::Uniform<uint64_t>(gen_);
        for (int j = 0; j < 32; ++j) {
            uint32_t &entry = buckets_[index].entries[seed & 3];
            seed >>= 2;
            std::swap(fp32, entry);
            index ^= fp32 & num_buckets_mask_;
            if (add(fp32, buckets_[index])) {
                ++size_;
                return true;
            }
        }
    }
    return false;
}

bool HashFilter32::test(uint64_t fingerprint) const {
    uint32_t fp32 = fingerprint;
    if (ABSL_PREDICT_FALSE(!fp32)) {
        fp32 = (fingerprint >> 32) | 1;
    }
    uint32_t index = (fingerprint >> 32) & num_buckets_mask_;
    return find_two(
        buckets_[index],
        buckets_[index ^ (fp32 & num_buckets_mask_)],
        fp32);
}

bool HashFilter32::add(uint32_t fp32, Bucket &bucket) {
    for (uint32_t &entry : bucket.entries) {
        if (ABSL_PREDICT_TRUE(!entry)) {
            entry = fp32;
            return true;
        }
    }
    return false;
}

bool HashFilter32::find_two(const Bucket &b0, const Bucket &b1, uint32_t fp32) {
#if defined(__AVX2__)
    __m256i a = _mm256_loadu2_m128i(
        reinterpret_cast<const __m128i *>(b0.entries.data()),
        reinterpret_cast<const __m128i *>(b1.entries.data()));
    __m256i b = _mm256_set1_epi32(fp32);
    return _mm256_movemask_epi8(_mm256_cmpeq_epi32(a, b));
#elif defined(__SSE2__)
    __m128i a0 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(b0.entries.data()));
    __m128i a1 = _mm_loadu_si128(
        reinterpret_cast<const __m128i *>(b1.entries.data()));
    __m128i b = _mm_set1_epi32(fp32);
    __m128i c0 = _mm_cmpeq_epi32(a0, b);
    __m128i c1 = _mm_cmpeq_epi32(a1, b);
    return _mm_movemask_epi8(_mm_or_si128(c0, c1));
#elif defined(__ARM_NEON)
    uint32x4_t a0 = vld1q_u32(b0.entries.data());
    uint32x4_t a1 = vld1q_u32(b1.entries.data());
    uint32x4_t b = vdupq_n_u32(fp32);
    uint32x4_t c0 = vceqq_u32(a0, b);
    uint32x4_t c1 = vceqq_u32(a1, b);
    return vaddvq_u32(vorrq_u32(c0, c1));
#else
    for (uint32_t entry : b0.entries) {
        if (entry == fp32) {
            return true;
        }
    }
    for (uint32_t entry : b1.entries) {
        if (entry == fp32) {
            return true;
        }
    }
    return false;
#endif
}

}  // namespace util
