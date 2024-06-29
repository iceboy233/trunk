#include "util/hex-dumper.h"

#include <string_view>
#include <system_error>
#include <gtest/gtest.h>

#include "base/types.h"
#include "io/memory-file.h"

namespace util {
namespace {

TEST(DumpHexTest, basic) {
    HexDumper dumper(7);
    io::MemoryFile file;
    EXPECT_EQ(dumper.dump(12, "hello world\n", file), std::error_code());
    EXPECT_EQ(std::string_view(ConstBufferSpan(file.content())),
              "0000000c: 6865 6c6c 6f20 77  hello w\n"
              "00000013: 6f72 6c64 0a       orld.\n");
}

}  // namespace
}  // namespace util
