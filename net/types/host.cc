#include "net/types/host.h"

namespace net {

std::string Host::to_string() const {
    if (is_address()) {
        return address().to_string();
    } else {
        return name();
    }
}

std::optional<Host> Host::from_string(
    std::string_view string, std::optional<bool> address_v6_only) {
    if (string.empty()) {
        return std::nullopt;
    }
    boost::system::error_code ec;
    net::address address;
    if (!address_v6_only) {
        address = make_address(string, ec);
    } else if (!*address_v6_only) {
        address = make_address_v4(string, ec);
    } else {
        address = make_address_v6(string, ec);
        if (ec) {
            return std::nullopt;
        }
    }
    if (!ec) {
        return address;
    }
    if (string.find_first_of("@#$%^&*()+={}[]|\\:;\"'<>,?/~ ") !=
        std::string_view::npos) {
        return std::nullopt;
    }
    return string;
}

bool operator==(const Host &a, const Host &b) {
    return a.rep_ == b.rep_;
}

bool operator!=(const Host &a, const Host &b) {
    return a.rep_ != b.rep_;
}

bool AbslParseFlag(std::string_view in, Host *out, std::string *error) {
    std::optional<Host> host = Host::from_string(in);
    if (!host) {
        return false;
    }
    *out = *host;
    return true;
}

std::string AbslUnparseFlag(const Host &in) {
    return in.to_string();
}

}  // namespace net
