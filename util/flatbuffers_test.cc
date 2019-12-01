#include "util/flatbuffers.h"

#include <cstdint>
#include <vector>
#include <gtest/gtest.h>
#include "util/flatbuffers/test_generated.h"

namespace util {
namespace {

TEST(FlatbuffersTest, pack_verify_unpack) {
    fbs::TestTableT object;
    object.dummy = 42;
    std::vector<uint8_t> buffer;
    pack<fbs::TestTable>(object, buffer);
    fbs::TestTableT object2;
    ASSERT_TRUE(verify_and_unpack<fbs::TestTable>(buffer, object2));
    EXPECT_EQ(object2.dummy, 42);
}

}  // namespace
}  // namespace util
