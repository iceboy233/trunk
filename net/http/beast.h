#ifndef _NET_HTTP_BEAST_H
#define _NET_HTTP_BEAST_H

#include <cstdint>
#include <boost/beast/http.hpp>

namespace net {
namespace http {

using boost::beast::http::field;
using boost::beast::http::status;
using boost::beast::http::verb;
using Request = boost::beast::http::request<
    boost::beast::http::vector_body<uint8_t>>;
using RequestParser = boost::beast::http::parser<
    true, boost::beast::http::vector_body<uint8_t>>;
using Response = boost::beast::http::response<
    boost::beast::http::vector_body<uint8_t>>;
using ResponseParser = boost::beast::http::parser<
    false, boost::beast::http::vector_body<uint8_t>>;

}  // namespace http
}  // namespace net

#endif  // _NET_HTTP_BEAST_H
