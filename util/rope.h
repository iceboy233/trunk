#ifndef _UTIL_ROPE_H
#define _UTIL_ROPE_H

#include <memory>
#include <string_view>

#include "absl/random/random.h"

namespace util {

// This class is not thread-safe.
class Rope {
public:
    void insert(size_t index, std::string_view str);
    void erase(size_t index, size_t size);
    void copy(size_t index, size_t size, char *dest) const;

private:
    static constexpr size_t buffer_size = 4096;

    struct Rep {
        std::unique_ptr<Rep> lchild;
        std::unique_ptr<Rep> rchild;
        size_t lsize = 0;
        uint32_t size = 0;
        uint32_t priority;
        char buffer[buffer_size];
    };

    std::unique_ptr<Rep> insert(
        std::unique_ptr<Rep> rep, size_t index,
        const char *buffer, size_t size);
    static std::unique_ptr<Rep> erase(
        std::unique_ptr<Rep> rep, size_t index, size_t size);
    static void copy(const Rep &rep, size_t index, size_t size, char *dest);
    static std::unique_ptr<Rep> merge(
        std::unique_ptr<Rep> lrep, std::unique_ptr<Rep> rrep);
    static std::unique_ptr<Rep> balance_left(std::unique_ptr<Rep> rep);
    static std::unique_ptr<Rep> balance_right(std::unique_ptr<Rep> rep);
    static std::unique_ptr<Rep> rotate_left(std::unique_ptr<Rep> rep);
    static std::unique_ptr<Rep> rotate_right(std::unique_ptr<Rep> rep);

    std::unique_ptr<Rep> rep_;
    absl::InsecureBitGen gen_;
};

}  // namespace util

#endif  // _UTIL_ROPE_H
