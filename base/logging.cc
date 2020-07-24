#include "base/logging.h"

#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

namespace base {
namespace {
namespace expr = boost::log::expressions;

const auto date_time_formatter =
    expr::stream <<
        expr::format_date_time<boost::posix_time::ptime>(
            "TimeStamp", "%m%d %H:%M:%S.%f");

const auto console_log_formatter = [](
    const boost::log::record_view &record,
    boost::log::formatting_ostream &stream) {
    auto severity = record[boost::log::trivial::severity];
    switch (severity.get()) {
    case boost::log::trivial::severity_level::trace:
        stream << "\033[37mT";
        break;
    case boost::log::trivial::severity_level::debug:
        stream << "\033[34mD";
        break;
    case boost::log::trivial::severity_level::info:
        stream << "\033[32mI";
        break;
    case boost::log::trivial::severity_level::warning:
        stream << "\033[33mW";
        break;
    case boost::log::trivial::severity_level::error:
        stream << "\033[31mE";
        break;
    case boost::log::trivial::severity_level::fatal:
        stream << "\033[30;41mF";
        break;
    }
    date_time_formatter(record, stream);
    stream << ' ' << record[detail::a_source_file] << ':'
           << record[detail::a_source_line] << "\033[0m "
           << record[expr::smessage];
};

}  // namespace

void init_logging() {
    boost::log::add_common_attributes();
    boost::log::add_console_log()->set_formatter(console_log_formatter);
}

}  // namespace base
