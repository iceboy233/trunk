#ifndef _BASE_LOGGING_H
#define _BASE_LOGGING_H

#include <cstdint>
#include <string_view>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

namespace base {

void init_logging();

namespace detail {

BOOST_LOG_ATTRIBUTE_KEYWORD(a_source_file, "Source File", std::string_view)
BOOST_LOG_ATTRIBUTE_KEYWORD(a_source_line, "Source Line", uint_fast32_t)

}  // namespace detail
}  // namespace base

#define LOG(severity) \
    BOOST_LOG_TRIVIAL(severity) \
        << ::boost::log::add_value(::base::detail::a_source_file, __FILE__) \
        << ::boost::log::add_value(::base::detail::a_source_line, __LINE__)

#endif  // _BASE_LOGGING_H
