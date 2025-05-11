#include "net/types/host-port.h"

#include "absl/strings/str_cat.h"
#include "util/strings.h"

namespace net {

std::string HostPort::to_string() const {
    if (is_addr_port()) {
        if (address().is_v4()) {
            return absl::StrCat(address().to_string(), ":", port_);
        } else {
            return absl::StrCat("[", address().to_string(), "]:", port_);
        }
    } else {
        return absl::StrCat(name(), ":", port_);
    }
}

std::optional<HostPort> HostPort::from_string(std::string_view string) {
    size_t pos = string.rfind(':');
    if (pos == std::string_view::npos || pos + 1 == string.size()) {
        return std::nullopt;
    }
    std::optional<Host> host;
    if (pos >= 2 && string[0] == '[' && string[pos - 1] == ']') {
        host = Host::from_string(string.substr(1, pos - 2), true);
    } else {
        host = Host::from_string(string.substr(0, pos), false);
    }
    if (!host) {
        return std::nullopt;
    }
    std::string_view port_str = string.substr(pos + 1);
    uint16_t port = util::consume_uint16(port_str);
    if (!port_str.empty()) {
        return std::nullopt;
    }
    return HostPort(*host, port);
}

bool operator==(const HostPort &a, const HostPort &b) {
    return a.host_ == b.host_ && a.port_ == b.port_;
}

bool operator!=(const HostPort &a, const HostPort &b) {
    return !(a == b);
}

bool AbslParseFlag(std::string_view in, HostPort *out, std::string *error) {
    std::optional<HostPort> host_port = HostPort::from_string(in);
    if (!host_port) {
        return false;
    }
    *out = *host_port;
    return true;
}

std::string AbslUnparseFlag(const HostPort &in) {
    return in.to_string();
}

}  // namespace net
