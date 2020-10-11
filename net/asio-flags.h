#ifndef _NET_ASIO_FLAGS_H
#define _NET_ASIO_FLAGS_H

#include <string>
#include <string_view>
#include <boost/asio.hpp>

namespace boost {
namespace asio {
namespace ip {

bool AbslParseFlag(std::string_view in, address *out, std::string *error);
bool AbslParseFlag(std::string_view in, address_v4 *out, std::string *error);
bool AbslParseFlag(std::string_view in, address_v6 *out, std::string *error);

std::string AbslUnparseFlag(const address &in);
std::string AbslUnparseFlag(const address_v4 &in);
std::string AbslUnparseFlag(const address_v6 &in);

}  // namespace ip
}  // namespace asio
}  // namespace boost

#endif  // _NET_ASIO_FLAGS_H
