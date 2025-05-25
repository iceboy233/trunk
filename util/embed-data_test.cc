#include "util/embed-data.h"

#include <array>
#include <string_view>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "base/types.h"

namespace util {
namespace {

using ::testing::ElementsAre;
using ::testing::Optional;

MATCHER_P(IsSpan, value, "") {
    return arg.data() == value.data() && arg.size() == value.size();
}

TEST(EmbeddedDataTest, All) {
    std::array<std::string_view, 2> names{{"name1", "name3"}};
    ConstBufferSpan content1 = "content1";
    ConstBufferSpan content3 = "content3";
    std::array<ConstBufferSpan, 2> contents{{content1, content3}};
    EmbedData data(names, contents);
    EXPECT_THAT(data.names(), ElementsAre("name1", "name3"));
    EXPECT_EQ(data.get_content_by_name("name0"), std::nullopt);
    EXPECT_THAT(data.get_content_by_name("name1"), Optional(IsSpan(content1)));
    EXPECT_EQ(data.get_content_by_name("name2"), std::nullopt);
    EXPECT_THAT(data.get_content_by_name("name3"), Optional(IsSpan(content3)));
    EXPECT_EQ(data.get_content_by_name("name4"), std::nullopt);
}

}  // namespace
}  // namespace util
