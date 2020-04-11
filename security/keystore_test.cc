#include "security/keystore.h"

#include <gtest/gtest.h>

namespace security {
namespace {

TEST(Keystore, Find) {
    Key key{{{}}};
    Key key1{{{1}}};
    Keystore keystore;
    keystore.add(key);
    keystore.add(key1);
    std::vector<Key> keys;

    keystore.find(0, keys);
    EXPECT_TRUE(keys.empty());

    keystore.find(0x903df1a0ade0b876, keys);
    ASSERT_EQ(keys.size(), 1);
    EXPECT_EQ(keys.front(), key);

    keystore.find(0x9311ece17c0ad3c5, keys);
    ASSERT_EQ(keys.size(), 1);
    EXPECT_EQ(keys.front(), key1);

    keystore.find(0, keys);
    EXPECT_TRUE(keys.empty());
}

}  // namespace
}  // namespace security
