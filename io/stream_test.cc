#include "io/stream.h"

#include "io/memory-file.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace io {
namespace {

using ::testing::ElementsAre;

TEST(OStreamTest, main) {
    MemoryFile file;
    OStream os(file);
    os << "foobar" << std::flush;
    EXPECT_THAT(file.content(), ElementsAre('f', 'o', 'o', 'b', 'a', 'r'));
}

}  // namespace
}  // namespace io
