#include "util/hash-filter.h"

#include <gtest/gtest.h>

namespace util {
namespace {

using testing::Values;

class HashFilter32Test : public testing::TestWithParam<uint64_t> {};

TEST_P(HashFilter32Test, single) {
    HashFilter32 filter(1);
    EXPECT_FALSE(filter.test(GetParam()));
    ASSERT_TRUE(filter.insert(GetParam()));
    EXPECT_TRUE(filter.test(GetParam()));
    filter.clear();
    EXPECT_FALSE(filter.test(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(, HashFilter32Test, Values(0, 1, 2, 42));

}  // namespace
}  // namespace util
