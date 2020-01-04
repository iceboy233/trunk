#include "util/flatbuffers-reflection.h"

#include "flatbuffers/reflection.h"

namespace util {

bool pack(
    flatbuffers::Parser &parser,
    const char *in,
    std::vector<uint8_t> &out) {
    if (!parser.Parse(in)) {
        return false;
    }
    out.assign(
        parser.builder_.GetBufferPointer(),
        parser.builder_.GetBufferPointer() + parser.builder_.GetSize());
    return true;
}

bool verify_and_unpack(
    flatbuffers::Parser &parser,
    absl::Span<const uint8_t> in,
    std::string &out) {
    parser.Serialize();
    const reflection::Schema *schema = reflection::GetSchema(
        parser.builder_.GetBufferPointer());
    if (!flatbuffers::Verify(
        *schema, *schema->root_table(), in.data(), in.size())) {
        return false;
    }
    return flatbuffers::GenerateText(parser, in.data(), &out);
}

}  // namespace util
