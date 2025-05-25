#ifndef _UTIL_EMBED_DATA_H
#define _UTIL_EMBED_DATA_H

#include <optional>
#include <string_view>

#include "absl/types/span.h"
#include "base/types.h"

namespace util {

class EmbedData {
public:
    EmbedData(
        absl::Span<const std::string_view> names,
        absl::Span<const ConstBufferSpan> contents);

    absl::Span<const std::string_view> names() const { return names_; }
    std::optional<ConstBufferSpan> get_content_by_name(
        std::string_view name) const;

private:
    absl::Span<const std::string_view> names_;
    absl::Span<const ConstBufferSpan> contents_;
};

}  // namespace util

#endif  // _UTIL_EMBED_DATA_H
