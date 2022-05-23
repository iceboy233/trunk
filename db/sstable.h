#ifndef _DB_SSTABLE_H
#define _DB_SSTABLE_H

#include <cstdint>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include "boost/container/flat_map.hpp"
#include "io/file.h"

namespace db {

class SSTable {
public:
    struct Options {};

    SSTable(io::File &file, const Options &options);

    std::error_code init();
    std::error_code lookup(std::string_view key, std::string &value) const;

private:
    struct Block {
        int64_t offset;
        int32_t size;
    };

    using BlockMap = boost::container::flat_map<std::string_view, Block>;

public:
    class Iterator {
    public:
        explicit Iterator(const SSTable &sstable);

        std::error_code start();
        std::error_code seek(std::string_view key);
        std::error_code next();
        std::string_view key() const;
        std::string_view value() const;

    private:
        std::error_code read();

        const SSTable &sstable_;
        BlockMap::const_iterator block_iter_;
        std::vector<uint8_t> buffer_;
        size_t offset_ = 0;
    };

private:
    io::File &file_;
    std::vector<uint8_t> index_buffer_;
    BlockMap blocks_;
};

class SSTableBuilder {
public:
    struct Options {
        int32_t block_size = 4096;
        size_t flush_size = 65536;
    };

    SSTableBuilder(io::File &file, const Options &options);

    std::error_code add(std::string_view key, std::string_view value);
    std::error_code finish();

private:
    std::error_code flush();

    struct Block {
        int64_t offset;
        int32_t size;
        std::string last;
    };

    io::File &file_;
    int32_t block_size_;
    size_t flush_size_;
    std::vector<uint8_t> buffer_;
    std::vector<Block> blocks_;
};

}  // namespace db

#endif  // _DB_SSTABLE_H
