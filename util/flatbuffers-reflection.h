#ifndef _UTIL_FLATBUFFERS_REFLECTION_H
#define _UTIL_FLATBUFFERS_REFLECTION_H

#include <cstdint>
#include <vector>
#include "absl/types/span.h"
#include "flatbuffers/idl.h"

namespace util {

bool pack(
    flatbuffers::Parser &parser,
    const char *in,
    std::vector<uint8_t> &out);

bool verify_and_unpack(
    flatbuffers::Parser &parser,
    absl::Span<const uint8_t> in,
    std::string &out);

}  // namespace util

#endif  // _UTIL_FLATBUFFERS_REFLECTION_H
