#ifndef _NET_TYPES_RESOLVER_H
#define _NET_TYPES_RESOLVER_H

#include <string_view>
#include <system_error>
#include <tuple>
#include <vector>

#include "net/asio.h"

namespace net {

class Resolver {
public:
    virtual ~Resolver() = default;

    virtual awaitable<std::tuple<std::error_code, std::vector<address>>>
    resolve(std::string_view name) = 0;
};

}  // namespace net

#endif  // _NET_TYPES_RESOLVER_H
