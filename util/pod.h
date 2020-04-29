#ifndef _UTIL_POD_H
#define _UTIL_POD_H

#include <new>
#include <type_traits>
#include <utility>

namespace util {

// Wraps a type into a POD with explicit creation and deletion.
template <typename T>
class Pod {
public:
    template <typename ...ArgsT>
    void emplace(ArgsT &&...args) {
        new(&storage_) T(std::forward<ArgsT>(args)...);
    }

    void destroy() { reinterpret_cast<T *>(&storage_)->~T(); }
    T &get() { return *reinterpret_cast<T *>(&storage_); }
    const T &get() const { return *reinterpret_cast<const T *>(&storage_); }
    T *operator->() { return &get(); }
    const T *operator->() const { return &get(); }

private:
    typename std::aligned_storage<sizeof(T), alignof(T)>::type storage_;
};

}  // namespace util

#endif  // _UTIL_POD_H
