#include "security/key.h"

#include <openssl/aead.h>
#include <openssl/chacha.h>
#include <limits>

namespace security {
namespace {

uint64_t compute_fingerprint(const KeyArray &array) {
    std::array<uint8_t, 8> fingerprint{};
    std::array<uint8_t, 12> nonce{};
    CRYPTO_chacha_20(
        fingerprint.data(), fingerprint.data(), fingerprint.size(),
        array.data(), nonce.data(), 0);
    return *reinterpret_cast<uint64_t *>(fingerprint.data());
}

}  // namespace

class Key::Impl {
public:
    Impl(const KeyArray &array);
    Impl(const Impl &) = delete;
    Impl &operator=(const Impl &) = delete;
    ~Impl();

    uint64_t fingerprint() const { return fingerprint_; }
    const KeyArray &array() const { return array_; }
    const EVP_AEAD_CTX &context() const { return context_; }

private:
    uint64_t fingerprint_;
    KeyArray array_;
    EVP_AEAD_CTX context_;
};

Key::Key(const KeyArray &array) : impl_(std::make_shared<Impl>(array)) {}

size_t Key::encrypt(
    absl::Span<const uint8_t> in,
    const uint8_t nonce[12],
    uint8_t *out) const {
    size_t out_size;
    EVP_AEAD_CTX_seal(
        &impl_->context(), out, &out_size, in.size() + encrypt_overhead,
        nonce, 12, in.data(), in.size(), nullptr, 0);
    return out_size;
}

size_t Key::decrypt(
    absl::Span<const uint8_t> in,
    const uint8_t nonce[12],
    uint8_t *out) const {
    size_t out_size;
    if (!EVP_AEAD_CTX_open(
        &impl_->context(), out, &out_size, in.size(),
        nonce, 12, in.data(), in.size(), nullptr, 0)) {
        return std::numeric_limits<size_t>::max();
    }
    return out_size;
}

uint64_t Key::fingerprint() const { return impl_->fingerprint(); }
const KeyArray &Key::array() const { return impl_->array(); }

bool operator==(const Key &a, const Key &b) {
    return a.array() == b.array();
}

bool operator!=(const Key &a, const Key &b) {
    return a.array() != b.array();
}

Key::Impl::Impl(const KeyArray &array)
    : fingerprint_(compute_fingerprint(array)), array_(array) {
    EVP_AEAD_CTX_init(
        &context_, EVP_aead_chacha20_poly1305(), array_.data(), array_.size(),
        EVP_AEAD_DEFAULT_TAG_LENGTH, nullptr);
}

Key::Impl::~Impl() { EVP_AEAD_CTX_cleanup(&context_); }

}  // namespace security
