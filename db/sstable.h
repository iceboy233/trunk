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

    io::File &file_;
    std::vector<uint8_t> index_buffer_;
    boost::container::flat_map<std::string_view, Block> blocks_;
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
        std::string start;
    };

    io::File &file_;
    int32_t block_size_;
    size_t flush_size_;
    std::vector<uint8_t> buffer_;
    std::vector<Block> blocks_;
};

}  // namespace db

#endif  // _DB_SSTABLE_H
