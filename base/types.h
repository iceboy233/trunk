#ifndef _BASE_TYPES_H
#define _BASE_TYPES_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

#include "absl/types/span.h"

class BufferSpan : public absl::Span<uint8_t> {
public:
    constexpr BufferSpan() = default;
    BufferSpan(void *data, size_t size)
        : Span(reinterpret_cast<uint8_t *>(data), size) {}

    BufferSpan(absl::Span<uint8_t> span) : Span(span) {}
    BufferSpan(std::vector<uint8_t> &v) : Span(v) {}

    template <size_t N>
    constexpr BufferSpan(std::array<uint8_t, N> &a) : Span(a) {}

    BufferSpan(std::string &s) : BufferSpan(s.data(), s.size()) {}

    template <size_t N>
    BufferSpan(std::array<char, N> &a) : BufferSpan(a.data(), N) {}

    operator std::string_view() const {
        return std::string_view(reinterpret_cast<char *>(data()), size());
    }
};

class ConstBufferSpan : public absl::Span<const uint8_t> {
public:
    constexpr ConstBufferSpan() = default;
    ConstBufferSpan(const void *data, size_t size)
        : Span(reinterpret_cast<const uint8_t *>(data), size) {}

    constexpr ConstBufferSpan(absl::Span<const uint8_t> span) : Span(span) {}
    constexpr ConstBufferSpan(absl::Span<uint8_t> span) : Span(span) {}
    constexpr ConstBufferSpan(const std::vector<uint8_t> &v) : Span(v) {}

    template <size_t N>
    constexpr ConstBufferSpan(const std::array<uint8_t, N> &a) : Span(a) {}

    ConstBufferSpan(const std::string &s)
        : ConstBufferSpan(s.data(), s.size()) {}

    ConstBufferSpan(std::string_view s) : ConstBufferSpan(s.data(), s.size()) {}
    ConstBufferSpan(const char *c) : ConstBufferSpan(std::string_view(c)) {}

    template <size_t N>
    ConstBufferSpan(const std::array<char, N> &a)
        : ConstBufferSpan(a.data(), N) {}

    operator std::string_view() const {
        return std::string_view(reinterpret_cast<const char *>(data()), size());
    }
};

#endif  // _BASE_TYPES_H
