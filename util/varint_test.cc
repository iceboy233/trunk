#include "util/varint.h"

#include <array>
#include <cstdint>
#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "absl/types/span.h"

namespace util {
namespace {

using ::testing::ElementsAre;

TEST(VarintTest, append) {
    std::vector<uint8_t> output;
    append_varint(3, output);
    EXPECT_THAT(output, ElementsAre(3));

    append_varint(270, output);
    EXPECT_THAT(output, ElementsAre(3, 142, 2));

    append_varint(86942, output);
    EXPECT_THAT(output, ElementsAre(3, 142, 2, 158, 167, 5));
}

TEST(VarintTest, parse) {
    std::array<uint8_t, 6> data{{3, 142, 2, 158, 167, 5}};
    absl::Span<const uint8_t> input(data);
    int32_t output;

    ASSERT_TRUE(parse_varint(input, output));
    EXPECT_EQ(output, 3);

    ASSERT_TRUE(parse_varint(input, output));
    EXPECT_EQ(output, 270);

    ASSERT_TRUE(parse_varint(input, output));
    EXPECT_EQ(output, 86942);

    ASSERT_FALSE(parse_varint(input, output));
}

}  // namespace
}  // namespace util
