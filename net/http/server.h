#include <chrono>
#include <functional>
#include <system_error>

#include "net/asio.h"
#include "net/http/beast.h"
#include "net/timer-list.h"

namespace net {
namespace http {

class Server {
public:
    using Handler = std::function<void(
        const Request &request,
        Response &response,
        std::function<void(std::error_code)> callback)>;

    struct Options {
        std::chrono::nanoseconds connection_timeout = std::chrono::minutes(1);
    };

    Server(
        const any_io_executor &executor,
        const tcp::endpoint &endpoint,
        Handler handler,
        const Options &options);

    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

private:
    template <typename StreamT>
    class Connection;

    void accept();

    tcp::acceptor acceptor_;
    TimerList timer_list_;
    Handler handler_;
};

}  // namespace http
}  // namespace net
