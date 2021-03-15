#ifndef _NET_ASIO_H
#define _NET_ASIO_H

#include <boost/asio.hpp>

namespace net {

using boost::asio::any_io_executor;
using boost::asio::async_connect;
using boost::asio::async_read;
using boost::asio::async_read_until;
using boost::asio::async_write;
using boost::asio::basic_waitable_timer;
using boost::asio::bind_executor;
using boost::asio::buffer;
using boost::asio::buffer_copy;
using boost::asio::buffered_read_stream;
using boost::asio::buffered_stream;
using boost::asio::buffered_write_stream;
using boost::asio::buffers_begin;
using boost::asio::buffers_end;
using boost::asio::connect;
using boost::asio::const_buffer;
using boost::asio::dispatch;
using boost::asio::defer;
using boost::asio::io_context;
using boost::asio::mutable_buffer;
using boost::asio::post;
using boost::asio::prefer;
using boost::asio::read;
using boost::asio::require;
using boost::asio::signal_set;
using boost::asio::socket_base;
using boost::asio::static_thread_pool;
using boost::asio::steady_timer;
using boost::asio::streambuf;
using boost::asio::transfer_all;
using boost::asio::transfer_at_least;
using boost::asio::transfer_exactly;
using boost::asio::wait_traits;
using boost::asio::write;
using boost::asio::ip::address;
using boost::asio::ip::address_v4;
using boost::asio::ip::address_v6;
using boost::asio::ip::icmp;
using boost::asio::ip::make_address;
using boost::asio::ip::make_address_v4;
using boost::asio::ip::make_address_v6;
using boost::asio::ip::tcp;
using boost::asio::ip::udp;

namespace execution = boost::asio::execution;

}  // namespace net

#endif  // _NET_ASIO_H
