#ifndef _NET_INTERFACE_RESOLVER_H
#define _NET_INTERFACE_RESOLVER_H

#include <string_view>
#include <system_error>

#include "absl/functional/any_invocable.h"
#include "absl/types/span.h"
#include "net/asio.h"

namespace net {

class Resolver {
public:
    virtual ~Resolver() = default;

    virtual void resolve(
        std::string_view name,
        absl::AnyInvocable<void(
            std::error_code, absl::Span<address const>) &&> callback) = 0;
};

}  // namespace net

#endif  // _NET_INTERFACE_RESOLVER_H
