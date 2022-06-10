#include "db/sstable.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "io/posix/file.h"

namespace db {
namespace {

TEST(SSTableTest, basic) {
    std::string filename = absl::StrCat(getenv("TEST_TMPDIR"), "/test");

    io::posix::File file;
    std::error_code ec = file.open(
        filename.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    ASSERT_FALSE(ec);

    // Create a SSTable.
    SSTableBuilder::Options options;
    options.block_size = 64;
    options.flush_size = 512;
    SSTableBuilder builder(file, options);
    for (int i = 1; i <= 100; ++i) {
        ec = builder.add(absl::StrFormat("k%03d", i),
                         absl::StrFormat("v%03d", i));
        ASSERT_FALSE(ec);
    }
    ec = builder.finish();
    ASSERT_FALSE(ec);
    file.close();

    // Open the SSTable.
    ec = file.open(filename.c_str(), O_RDONLY, 0);
    ASSERT_FALSE(ec);
    SSTable sstable(file, {});
    ec = sstable.init();
    ASSERT_FALSE(ec);

    // Test lookup.
    std::string value;
    for (int i = 1; i <= 100; ++i) {
        ec = sstable.lookup(absl::StrFormat("k%03d", i), value);
        ASSERT_FALSE(ec);
        EXPECT_EQ(value, absl::StrFormat("v%03d", i));
    }

    // Test iterate.
    SSTable::Iterator iterator(sstable);
    ASSERT_TRUE(iterator.next());
    ASSERT_FALSE(iterator.start());
    for (int i = 1; i < 100; ++i) {
        ASSERT_EQ(std::string_view(iterator.key()), absl::StrFormat("k%03d", i));
        EXPECT_EQ(std::string_view(iterator.value()), absl::StrFormat("v%03d", i));
        ASSERT_FALSE(iterator.next());
    }
    ASSERT_EQ(std::string_view(iterator.key()), "k100");
    EXPECT_EQ(std::string_view(iterator.value()), "v100");
    ASSERT_TRUE(iterator.next());

    // Test seek.
    for (int i = 0; i < 100; ++i) {
        SSTable::Iterator iterator(sstable);
        ec = iterator.seek(absl::StrFormat("k%03da", i));
        ASSERT_FALSE(ec);
        ASSERT_EQ(std::string_view(iterator.key()), absl::StrFormat("k%03d", i + 1));
        EXPECT_EQ(std::string_view(iterator.value()), absl::StrFormat("v%03d", i + 1));
    }
    ec = iterator.seek("k100a");
    ASSERT_TRUE(ec);
}

TEST(SSTableTest, empty) {
    std::string filename = absl::StrCat(getenv("TEST_TMPDIR"), "/test");

    io::posix::File file;
    std::error_code ec = file.open(
        filename.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    ASSERT_FALSE(ec);

    SSTableBuilder builder(file, {});
    ec = builder.finish();
    ASSERT_FALSE(ec);
    file.close();

    ec = file.open(filename.c_str(), O_RDONLY, 0);
    ASSERT_FALSE(ec);
    SSTable sstable(file, {});
    ec = sstable.init();
    ASSERT_FALSE(ec);

    std::string value;
    ec = sstable.lookup("k42", value);
    ASSERT_TRUE(ec);
    SSTable::Iterator iterator(sstable);
    ec = iterator.start();
    ASSERT_TRUE(ec);
    ec = iterator.next();
    ASSERT_TRUE(ec);
}

}  // namespace
}  // namespace db
