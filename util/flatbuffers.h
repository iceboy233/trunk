#ifndef _UTIL_FLATBUFFERS_H
#define _UTIL_FLATBUFFERS_H

#include <cstdint>
#include <vector>
#include "absl/types/span.h"
#include "flatbuffers/flatbuffers.h"

namespace util {

template <typename T>
void pack(
    const typename T::NativeTableType &object, std::vector<uint8_t> &buffer) {
    flatbuffers::FlatBufferBuilder fbb;
    fbb.Finish(T::Pack(fbb, &object));
    // TODO(iceboy): Remove the copy.
    buffer.assign(
        fbb.GetBufferPointer(), fbb.GetBufferPointer() + fbb.GetSize());
}

template <typename T>
bool verify_and_unpack(
    absl::Span<const uint8_t> buffer, typename T::NativeTableType &object) {
    flatbuffers::Verifier verifier(buffer.data(), buffer.size());
    if (!verifier.VerifyBuffer<T>()) {
        return false;
    }
    flatbuffers::GetRoot<T>(buffer.data())->UnPackTo(&object);
    return true;
}

}  // namespace util

#endif  // _UTIL_FLATBUFFERS_H
