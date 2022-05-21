#include "db/sstable.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "io/posix-file.h"

namespace db {
namespace {

TEST(SSTableTest, basic) {
    std::string filename = absl::StrCat(getenv("TEST_TMPDIR"), "/test");

    std::unique_ptr<io::File> file;
    std::error_code ec = io::open_posix_file(
        filename.c_str(), O_CREAT | O_WRONLY, 0644, file);
    ASSERT_FALSE(ec);

    SSTableBuilder::Options options;
    options.block_size = 64;
    options.flush_size = 512;
    SSTableBuilder builder(*file, options);
    for (int i = 1; i <= 100; ++i) {
        ec = builder.add(absl::StrFormat("k%03d", i),
                         absl::StrFormat("v%03d", i));
        ASSERT_FALSE(ec);
    }
    ec = builder.finish();
    ASSERT_FALSE(ec);
    file.reset();

    ec = io::open_posix_file(filename.c_str(), O_RDONLY, 0, file);
    ASSERT_FALSE(ec);
    SSTable sstable(*file, {});
    ec = sstable.init();
    ASSERT_FALSE(ec);
    std::string value;
    for (int i = 1; i <= 100; ++i) {
        ec = sstable.lookup(absl::StrFormat("k%03d", i), value);
        ASSERT_FALSE(ec);
        EXPECT_EQ(value, absl::StrFormat("v%03d", i));
    }
}

}  // namespace
}  // namespace db
