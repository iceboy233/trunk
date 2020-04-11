#include "security/key.h"

#include <vector>
#include <gtest/gtest.h>

namespace security {
namespace {

TEST(Key, Fingerprint) {
    EXPECT_EQ(Key{{}}.fingerprint(), 0x903df1a0ade0b876);
}

TEST(Key, Fingerprint1) {
    EXPECT_EQ(Key{{1}}.fingerprint(), 0x9311ece17c0ad3c5);
}

TEST(Key, DecryptInvalid) {
    Key key{{}};
    NonceArray nonce{};
    std::array<uint8_t, 16> buffer{};
    EXPECT_EQ(
        key.decrypt(buffer, nonce.data(), buffer.data()),
        std::numeric_limits<size_t>::max());
}

struct TestVector {
    std::vector<uint8_t> plain;
    std::vector<uint8_t> crypt;
};

class VectorTest : public testing::TestWithParam<TestVector> {};

TEST_P(VectorTest, Encrypt) {
    const TestVector &vector = GetParam();
    Key key{{}};
    NonceArray nonce{};
    std::vector<uint8_t> output(vector.plain.size() + Key::encrypt_overhead);
    EXPECT_EQ(key.encrypt(vector.plain, nonce.data(), output.data()),
              vector.crypt.size());
    output.resize(vector.crypt.size());
    EXPECT_EQ(output, vector.crypt);
}

TEST_P(VectorTest, Decrypt) {
    const TestVector &vector = GetParam();
    Key key{{}};
    NonceArray nonce{};
    std::vector<uint8_t> output(vector.crypt.size());
    EXPECT_EQ(key.decrypt(vector.crypt, nonce.data(), output.data()),
              vector.plain.size());
    output.resize(vector.plain.size());
    EXPECT_EQ(output, vector.plain);
}

INSTANTIATE_TEST_CASE_P(VectorTest, VectorTest, testing::Values(
    TestVector{
        {},
        {
            0x4e, 0xb9, 0x72, 0xc9, 0xa8, 0xfb, 0x3a, 0x1b,
            0x38, 0x2b, 0xb4, 0xd3, 0x6f, 0x5f, 0xfa, 0xd1,
        },
    },
    TestVector{
        {1, 2, 3, 4, 5},
        {
            0x9e, 0x05, 0xe4, 0xba, 0x50, 0xfa, 0x7c, 0xdb, 
            0x85, 0x24, 0xdc, 0xa5, 0x8a, 0xfc, 0xe4, 0x39, 
            0x97, 0x9c, 0x7d, 0xf8, 0x10,
        },
    }
));

}  // namespace
}  // namespace security
