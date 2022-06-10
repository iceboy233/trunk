#include "io/memory-file.h"

#include <algorithm>
#include <utility>

namespace io {

MemoryFile::MemoryFile(std::vector<uint8_t> content)
    : content_(std::move(content)) {}

std::error_code MemoryFile::read(BufferSpan buffer, size_t &size) {
    if (position_ >= content_.size()) {
        size = 0;
        return {};
    }
    size_t last = std::min(position_ + buffer.size(), content_.size());
    std::copy(&content_[position_], &content_[last], buffer.data());
    size = last - position_;
    position_ = last;
    return {};
}

std::error_code MemoryFile::write(ConstBufferSpan buffer, size_t &size) {
    size_t last = position_ + buffer.size();
    if (last > content_.size()) {
        content_.resize(last);
    }
    std::copy_n(buffer.data(), buffer.size(), &content_[position_]);
    size = buffer.size();
    position_ += buffer.size();
    return {};
}

std::error_code MemoryFile::pread(
    int64_t position, BufferSpan buffer, size_t &size) {
    if (position < 0) {
        return make_error_code(std::errc::invalid_argument);
    }
    if (static_cast<size_t>(position) >= content_.size()) {
        size = 0;
        return {};
    }
    size_t last = std::min(position + buffer.size(), content_.size());
    std::copy(&content_[position], &content_[last], buffer.data());
    size = last - position;
    return {};
}

std::error_code MemoryFile::pwrite(
    int64_t position, ConstBufferSpan buffer, size_t &size) {
    if (position < 0) {
        return make_error_code(std::errc::invalid_argument);
    }
    size_t last = position + buffer.size();
    if (last > content_.size()) {
        content_.resize(last);
    }
    std::copy_n(buffer.data(), buffer.size(), &content_[position]);
    size = buffer.size();
    return {};
}

std::error_code MemoryFile::seek(int64_t position) {
    if (position < 0) {
        return make_error_code(std::errc::invalid_seek);
    }
    position_ = position;
    return {};
}

std::error_code MemoryFile::tell(int64_t &position) {
    position = position_;
    return {};
}

std::error_code MemoryFile::size(int64_t &size) {
    size = content_.size();
    return {};
}

}  // namespace io
