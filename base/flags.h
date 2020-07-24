#ifndef _BASE_FLAGS_H
#define _BASE_FLAGS_H

#include <functional>
#include "absl/flags/flag.h"

namespace base {

void parse_flags(int argc, char *argv[]);

namespace detail {

void register_flag(std::function<void()> setter);

}  // namespace detail
}  // namespace base

#define DECLARE_FLAG(_type, _name) \
    namespace flags { \
    extern _type _name; \
    } \

#define DEFINE_FLAG(_type, _name, _default_value, _description) \
    ABSL_FLAG(_type, _name, _default_value, _description); \
    namespace flags { \
    _type _name; \
    } \
    namespace flags_register { \
    namespace { \
    auto _name = []() { \
        base::detail::register_flag( \
            []() { flags::_name = absl::GetFlag(FLAGS_##_name); }); \
        return 0; \
    }(); \
    } \
    } \

#endif  // namespace _BASE_FLAGS_H
