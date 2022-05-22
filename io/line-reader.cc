#include "io/line-reader.h"

#include "absl/types/span.h"

namespace io {

LineReader::LineReader(io::File &file, const Options &options)
    : file_(file),
      delimiter_(options.delimiter) {
    buffer_.reserve(options.buffer_size);
}

std::error_code LineReader::read(std::string &line, bool &end_of_file) {
    line.clear();
    while (true) {
        while (offset_ < buffer_.size()) {
            line.push_back(static_cast<char>(buffer_[offset_++]));
            if (line.back() == delimiter_) {
                end_of_file = false;
                return {};
            }
        }
        buffer_.resize(buffer_.capacity());
        offset_ = 0;
        size_t size;
        std::error_code ec = file_.read(absl::MakeSpan(buffer_), size);
        if (ec) {
            return ec;
        }
        if (!size) {
            end_of_file = true;
            return {};
        }
        buffer_.resize(size);
    }
}

}  // namespace io
