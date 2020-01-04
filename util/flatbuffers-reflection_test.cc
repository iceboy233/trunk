#include "util/flatbuffers-reflection.h"

#include <gtest/gtest.h>
#include "flatbuffers/idl.h"

namespace util {
namespace {

TEST(FlatbuffersReflectionDynamicTest, pack_verify_unpack) {
    flatbuffers::Parser parser;
    ASSERT_TRUE(parser.Parse("table TestTable { dummy: int32; }"));
    ASSERT_TRUE(parser.SetRootType("TestTable"));
    std::vector<uint8_t> buffer;
    ASSERT_TRUE(pack(parser, "{dummy: 123}", buffer));
    std::string json;
    ASSERT_TRUE(verify_and_unpack(parser, buffer, json));
    EXPECT_EQ(json, "{\n  dummy: 123\n}\n");
}

}  // namespace
}  // namespace util
