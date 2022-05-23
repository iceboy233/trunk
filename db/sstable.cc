#include "db/sstable.h"

#include "boost/endian/conversion.hpp"
#include "boost/endian/buffers.hpp"

namespace db {
namespace {

constexpr int32_t max_block_size = 16777216;

}  // namespace

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
    Iterator iterator(*this);
    std::error_code ec = iterator.seek(key);
    if (ec) {
        return ec;
    }
    if (iterator.key() != key) {
        return make_error_code(std::errc::no_message_available);
    }
    value = iterator.value();
    return {};
}

SSTable::Iterator::Iterator(const SSTable &sstable)
    : sstable_(sstable) {}

std::error_code SSTable::Iterator::start() {
    if (sstable_.blocks_.empty()) {
        return make_error_code(std::errc::no_message_available);
    }
    block_iter_ = sstable_.blocks_.begin();
    offset_ = 0;
    return read();
}

std::error_code SSTable::Iterator::seek(std::string_view key) {
    auto iter = sstable_.blocks_.lower_bound(key);
    if (iter == sstable_.blocks_.end()) {
        return make_error_code(std::errc::no_message_available);
    }
    block_iter_ = iter;
    offset_ = 0;
    std::error_code ec = read();
    if (ec) {
        return ec;
    }
    while (this->key() < key) {
        ec = next();
        if (ec) {
            return ec;
        }
    }
    return {};
}

std::error_code SSTable::Iterator::next() {
    if (offset_ + 8 > buffer_.size()) {
        return make_error_code(std::errc::no_buffer_space);
    }
    int32_t key_size = boost::endian::load_little_s32(&buffer_[offset_]);
    int32_t value_size = boost::endian::load_little_s32(&buffer_[offset_ + 4]);
    if (offset_ + 8 + key_size + value_size > buffer_.size()) {
        return make_error_code(std::errc::no_buffer_space);
    }
    offset_ += 8 + key_size + value_size;
    if (offset_ == buffer_.size()) {
        if (++block_iter_ == sstable_.blocks_.end()) {
            return make_error_code(std::errc::no_message_available);
        }
        offset_ = 0;
        return read();
    }
    return {};
}

std::string_view SSTable::Iterator::key() const {
    int32_t key_size = boost::endian::load_little_s32(&buffer_[offset_]);
    return std::string_view(
        reinterpret_cast<const char *>(&buffer_[offset_ + 8]), key_size);
}

std::string_view SSTable::Iterator::value() const {
    int32_t key_size = boost::endian::load_little_s32(&buffer_[offset_]);
    int32_t value_size = boost::endian::load_little_s32(&buffer_[offset_ + 4]);
    return std::string_view(
        reinterpret_cast<const char *>(&buffer_[offset_ + 8 + key_size]),
        value_size);
}

std::error_code SSTable::Iterator::read() {
    const Block &block = block_iter_->second;
    if (block.size <= 0 || block.size > max_block_size) {
        return make_error_code(std::errc::bad_message);
    }
    buffer_.resize(block.size);
    size_t size;
    std::error_code ec = sstable_.file_.pread(
        block.offset, absl::MakeSpan(buffer_), size);
    if (ec) {
        return ec;
    }
    return {};
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
        blocks_.back().last = key;
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
        size_t entry_size = 16 + block.last.size();
        buffer_.resize(offset + entry_size);
        boost::endian::store_little_s64(&buffer_[offset], block.offset);
        boost::endian::store_little_s32(&buffer_[offset + 8], block.size);
        boost::endian::store_little_s32(
            &buffer_[offset + 12], static_cast<int32_t>(block.last.size()));
        std::copy(block.last.begin(), block.last.end(), &buffer_[offset + 16]);
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
