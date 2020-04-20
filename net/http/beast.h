#ifndef _NET_HTTP_BEAST_H
#define _NET_HTTP_BEAST_H

#include <cstdint>
#include <boost/beast/http.hpp>

namespace net {
namespace http {

using boost::beast::http::field;
using boost::beast::http::status;
using boost::beast::http::verb;
using Body = boost::beast::http::vector_body<uint8_t>;
using Request = boost::beast::http::request<Body>;
using RequestParser = boost::beast::http::request_parser<Body>;
using Response = boost::beast::http::response<Body>;
using ResponseParser = boost::beast::http::response_parser<Body>;

}  // namespace http
}  // namespace net

#endif  // _NET_HTTP_BEAST_H
