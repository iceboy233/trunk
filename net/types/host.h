#ifndef _NET_TYPES_HOST_H
#define _NET_TYPES_HOST_H

#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include "net/asio.h"

namespace net {

class Host {
public:
    Host() = default;
    Host(const net::address &address) : rep_(address) {}
    Host(const net::address_v4 &address) : rep_(address) {}
    Host(const net::address_v6 &address) : rep_(address) {}
    Host(std::string_view name) : rep_(std::string(name)) {}

    bool is_address() const {
        return std::holds_alternative<net::address>(rep_);
    }
    const net::address &address() const { return std::get<net::address>(rep_); }
    void set_address(const net::address &address) { rep_ = address; }

    bool is_name() const { return std::holds_alternative<std::string>(rep_); }
    const std::string &name() const { return std::get<std::string>(rep_); }
    void set_name(std::string_view name) { rep_ = std::string(name); }

    std::string to_string() const;
    static std::optional<Host> from_string(
        std::string_view string,
        std::optional<bool> address_v6_only = std::nullopt);

private:
    friend bool operator==(const Host &a, const Host &b);
    friend bool operator!=(const Host &a, const Host &b);

    template <typename H>
    friend H AbslHashValue(H h, const Host &host);

    std::variant<net::address, std::string> rep_;
};

template <typename H>
H AbslHashValue(H h, const Host &host) {
    return H::combine(std::move(h), host.rep_);
}

bool AbslParseFlag(std::string_view in, Host *out, std::string *error);
std::string AbslUnparseFlag(const Host &in);

}  // namespace net

#endif  // _NET_TYPES_HOST_H
