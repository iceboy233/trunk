#include "util/embed-data.h"

#include <algorithm>

namespace util {

EmbedData::EmbedData(
    absl::Span<const std::string_view> names,
    absl::Span<const ConstBufferSpan> contents)
    : names_(names),
      contents_(contents) {}

std::optional<ConstBufferSpan> EmbedData::get_content_by_name(
    std::string_view name) const {
    auto iter = std::lower_bound(names_.begin(), names_.end(), name);
    if (iter == names_.end() || *iter != name) {
        return std::nullopt;
    }
    return contents_[iter - names_.begin()];
}

}  // namespace util
