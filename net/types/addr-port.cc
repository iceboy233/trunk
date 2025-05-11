#include "net/types/addr-port.h"

#include "absl/strings/str_cat.h"
#include "util/strings.h"

namespace net {

std::string AddrPort::to_string() const {
    if (address_.is_v4()) {
        return absl::StrCat(address_.to_string(), ":", port_);
    } else {
        return absl::StrCat("[", address_.to_string(), "]:", port_);
    }
}

std::optional<AddrPort> AddrPort::from_string(std::string_view string) {
    size_t pos = string.rfind(':');
    if (pos == std::string_view::npos || pos + 1 == string.size()) {
        return std::nullopt;
    }
    boost::system::error_code ec;
    net::address address;
    if (pos >= 2 && string[0] == '[' && string[pos - 1] == ']') {
        address = make_address_v6(string.substr(1, pos - 2), ec);
    } else {
        address = make_address_v4(string.substr(0, pos), ec);
    }
    if (ec) {
        return std::nullopt;
    }
    std::string_view port_str = string.substr(pos + 1);
    uint16_t port = util::consume_uint16(port_str);
    if (!port_str.empty()) {
        return std::nullopt;
    }
    return AddrPort(address, port);
}

bool operator==(const AddrPort &a, const AddrPort &b) {
    return a.address_ == b.address_ && a.port_ == b.port_;
}

bool operator!=(const AddrPort &a, const AddrPort &b) {
    return !(a == b);
}

bool AbslParseFlag(std::string_view in, AddrPort *out, std::string *error) {
    std::optional<AddrPort> addr_port = AddrPort::from_string(in);
    if (!addr_port) {
        return false;
    }
    *out = *addr_port;
    return true;
}

std::string AbslUnparseFlag(const AddrPort &in) {
    return in.to_string();
}

}  // namespace net
