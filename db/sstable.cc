#include "db/sstable.h"

#include "boost/endian/conversion.hpp"
#include "boost/endian/buffers.hpp"

namespace db {

SSTable::SSTable(io::File &file, const Options &options)
    : file_(file) {}

std::error_code SSTable::init() {
    int64_t file_size;
    std::error_code ec = file_.size(file_size);
    if (ec) {
        return ec;
    }
    if (file_size < 8) {
        return make_error_code(std::errc::no_message);
    }

    boost::endian::little_int64_buf_t index_size_buffer;
    size_t size;
    ec = file_.pread(file_size - 8, {index_size_buffer.data(), 8}, size);
    int64_t index_size = index_size_buffer.value();
    if (file_size < 8 + index_size) {
        return make_error_code(std::errc::no_buffer_space);
    }
    index_buffer_.resize(index_size);
    ec = file_.pread(
        file_size - (8 + index_size), absl::MakeSpan(index_buffer_), size);
    if (ec) {
        return ec;
    }

    size_t offset = 0;
    while (offset != index_buffer_.size()) {
        if (offset + 16 > index_buffer_.size()) {
            return make_error_code(std::errc::no_buffer_space);
        }
        Block block;
        block.offset = boost::endian::load_little_s64(&index_buffer_[offset]);
        block.size = boost::endian::load_little_s32(&index_buffer_[offset + 8]);
        int32_t block_start_size =
            boost::endian::load_little_s32(&index_buffer_[offset + 12]);
        if (offset + 16 + block_start_size > index_buffer_.size()) {
            return make_error_code(std::errc::no_buffer_space);
        }
        std::string_view start(
            reinterpret_cast<char *>(&index_buffer_[offset + 16]),
            block_start_size);
        blocks_.insert(blocks_.end(), {start, block});
        offset += 16 + block_start_size;
    }
    return {};
}

std::error_code SSTable::lookup(
    std::string_view key, std::string &value) const {
    auto iter = blocks_.upper_bound(key);
    if (iter == blocks_.begin()) {
        return make_error_code(std::errc::no_message_available);
    }
    const Block &block = std::prev(iter)->second;
    std::vector<uint8_t> buffer(block.size);
    size_t size;
    std::error_code ec = file_.pread(
        block.offset, absl::MakeSpan(buffer), size);
    if (ec) {
        return ec;
    }

    size_t offset = 0;
    while (offset != buffer.size()) {
        if (offset + 8 > buffer.size()) {
            return make_error_code(std::errc::no_buffer_space);
        }
        int32_t key_size = boost::endian::load_little_s32(&buffer[offset]);
        int32_t value_size =
            boost::endian::load_little_s32(&buffer[offset + 4]);
        if (offset + 8 + key_size + value_size > buffer.size()) {
            return make_error_code(std::errc::no_buffer_space);
        }
        if (key == std::string_view(
                reinterpret_cast<char *>(&buffer[offset + 8]), key_size)) {
            value = std::string_view(
                reinterpret_cast<char *>(&buffer[offset + 8 + key_size]),
                value_size);
            return {};
        }
        offset += 8 + key_size + value_size;
    }
    return make_error_code(std::errc::no_message_available);
}

SSTableBuilder::SSTableBuilder(io::File &file, const Options &options)
    : file_(file),
      block_size_(options.block_size),
      flush_size_(options.flush_size) {}

std::error_code SSTableBuilder::add(
    std::string_view key, std::string_view value) {
    size_t offset = buffer_.size();
    int32_t entry_size = static_cast<int32_t>(8 + key.size() + value.size());
    buffer_.resize(offset + entry_size);
    boost::endian::store_little_s32(
        &buffer_[offset], static_cast<int32_t>(key.size()));
    boost::endian::store_little_s32(
        &buffer_[offset + 4], static_cast<int32_t>(value.size()));
    std::copy(key.begin(), key.end(), &buffer_[offset + 8]);
    std::copy(value.begin(), value.end(), &buffer_[offset + 8 + key.size()]);

    if (blocks_.empty()) {
        blocks_.push_back({0, entry_size, std::string(key)});
    } else if (blocks_.back().size + entry_size > block_size_) {
        int64_t block_offset = blocks_.back().offset + blocks_.back().size;
        blocks_.push_back({block_offset, entry_size, std::string(key)});
    } else {
        blocks_.back().size += entry_size;
    }

    if (buffer_.size() >= flush_size_) {
        std::error_code ec = flush();
        if (ec) {
            return ec;
        }
    }
    return {};
}

std::error_code SSTableBuilder::finish() {
    int64_t index_size = 0;
    for (const auto &block : blocks_) {
        size_t offset = buffer_.size();
        size_t entry_size = 16 + block.start.size();
        buffer_.resize(offset + entry_size);
        boost::endian::store_little_s64(&buffer_[offset], block.offset);
        boost::endian::store_little_s32(&buffer_[offset + 8], block.size);
        boost::endian::store_little_s32(
            &buffer_[offset + 12], static_cast<int32_t>(block.start.size()));
        std::copy(
            block.start.begin(), block.start.end(), &buffer_[offset + 16]);
        if (buffer_.size() >= flush_size_) {
            std::error_code ec = flush();
            if (ec) {
                return ec;
            }
        }
        index_size += entry_size;
    }
    size_t offset = buffer_.size();
    buffer_.resize(offset + 8);
    boost::endian::store_little_s64(&buffer_[offset], index_size);
    return flush();
}

std::error_code SSTableBuilder::flush() {
    size_t size;
    std::error_code ec = file_.write(buffer_, size);
    if (ec) {
        return ec;
    }
    buffer_.clear();
    return {};
}

}  // namespace db
