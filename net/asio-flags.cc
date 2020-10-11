#include "net/asio-flags.h"

#include <boost/system/error_code.hpp>

namespace boost {
namespace asio {
namespace ip {

bool AbslParseFlag(std::string_view in, address *out, std::string *error) {
    if (in.empty()) {
        *out = address();
        return true;
    }
    boost::system::error_code ec;
    *out = make_address(std::string(in), ec);
    if (ec) {
        *error = ec.message();
        return false;
    }
    return true;
}

bool AbslParseFlag(std::string_view in, address_v4 *out, std::string *error) {
    if (in.empty()) {
        *out = address_v4();
        return true;
    }
    boost::system::error_code ec;
    *out = make_address_v4(std::string(in), ec);
    if (ec) {
        *error = ec.message();
        return false;
    }
    return true;
}

bool AbslParseFlag(std::string_view in, address_v6 *out, std::string *error) {
    if (in.empty()) {
        *out = address_v6();
        return true;
    }
    boost::system::error_code ec;
    *out = make_address_v6(std::string(in), ec);
    if (ec) {
        *error = ec.message();
        return false;
    }
    return true;
}

std::string AbslUnparseFlag(const address &in) {
    return in.to_string();
}

std::string AbslUnparseFlag(const address_v4 &in) {
    return in.to_string();
}

std::string AbslUnparseFlag(const address_v6 &in) {
    return in.to_string();
}

}  // namespace ip
}  // namespace asio
}  // namespace boost
