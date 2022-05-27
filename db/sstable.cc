#include "db/sstable.h"

#include "absl/algorithm/container.h"
#include "boost/endian/conversion.hpp"

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
    std::array<uint8_t, 8> buffer;
    size_t size;
    ec = file_.pread(file_size - 8, absl::MakeSpan(buffer), size);
    int32_t num_blocks = boost::endian::load_little_s32(&buffer[0]);
    int32_t keys_size = boost::endian::load_little_s32(&buffer[4]);
    if (file_size < num_blocks * 16 + keys_size + 8) {
        return make_error_code(std::errc::no_buffer_space);
    }
    std::vector<uint8_t> blocks_buffer(num_blocks * 16);
    ec = file_.pread(
        file_size - (num_blocks * 16 + keys_size + 8),
        absl::MakeSpan(blocks_buffer), size);
    if (ec) {
        return ec;
    }
    keys_buffer_.resize(keys_size);
    ec = file_.pread(
        file_size - (keys_size + 8), absl::MakeSpan(keys_buffer_), size);
    if (ec) {
        return ec;
    }
    blocks_.reserve(num_blocks);
    keys_.reserve(num_blocks);
    size_t keys_offset = 0;
    for (int32_t index = 0; index < num_blocks; ++index) {
        blocks_.push_back({
            boost::endian::load_little_s64(&blocks_buffer[index * 16]),
            boost::endian::load_little_s32(&blocks_buffer[index * 16 + 8])});
        if (blocks_.back().size <= 0 || blocks_.back().size > max_block_size) {
            return make_error_code(std::errc::bad_message);
        }
        size_t key_size =
            boost::endian::load_little_s32(&blocks_buffer[index * 16 + 12]);
        keys_.push_back({reinterpret_cast<char *>(&keys_buffer_[keys_offset]),
                         key_size});
        keys_offset += key_size;
        if (keys_offset > keys_buffer_.size()) {
            return make_error_code(std::errc::no_buffer_space);
        }
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
    iter_ = sstable_.blocks_.begin();
    offset_ = 0;
    return read();
}

std::error_code SSTable::Iterator::seek(std::string_view key) {
    auto iter = absl::c_lower_bound(sstable_.keys_, key);
    if (iter == sstable_.keys_.end()) {
        return make_error_code(std::errc::no_message_available);
    }
    iter_ = sstable_.blocks_.begin() + (iter - sstable_.keys_.begin());
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
        if (++iter_ == sstable_.blocks_.end()) {
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
    const Block &block = *iter_;
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
    size_t size = buffer_.size();
    int32_t entry_size = 8 + key.size() + value.size();
    buffer_.resize(size + entry_size);
    boost::endian::store_little_s32(&buffer_[size], key.size());
    boost::endian::store_little_s32(&buffer_[size + 4], value.size());
    absl::c_copy(key, &buffer_[size + 8]);
    absl::c_copy(value, &buffer_[size + 8 + key.size()]);

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
    for (const auto &block : blocks_) {
        size_t size = buffer_.size();
        buffer_.resize(size + 16);
        boost::endian::store_little_s64(&buffer_[size], block.offset);
        boost::endian::store_little_s32(&buffer_[size + 8], block.size);
        boost::endian::store_little_s32(&buffer_[size + 12], block.last.size());
        if (buffer_.size() >= flush_size_) {
            std::error_code ec = flush();
            if (ec) {
                return ec;
            }
        }
    }
    int32_t keys_size = 0;
    for (const auto &block : blocks_) {
        size_t size = buffer_.size();
        buffer_.resize(size + block.last.size());
        absl::c_copy(block.last, &buffer_[size]);
        if (buffer_.size() >= flush_size_) {
            std::error_code ec = flush();
            if (ec) {
                return ec;
            }
        }
        keys_size += block.last.size();
    }

    size_t size = buffer_.size();
    buffer_.resize(size + 8);
    boost::endian::store_little_s32(&buffer_[size], blocks_.size());
    boost::endian::store_little_s32(&buffer_[size + 4], keys_size);
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
