#include "net/icmp-client.h"

#include <boost/process/environment.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include "base/packed.h"

namespace net {
namespace {

PACKED_BEGIN
struct PACKED IcmpHeader {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence_number;
};
PACKED_END
static_assert(sizeof(IcmpHeader) == 8, "");

constexpr uint8_t echo_reply = 0;
constexpr uint8_t echo_request = 8;

void compute_checksum(uint8_t *data, size_t size, IcmpHeader &header) {
    uint32_t sum = (header.type << 8) | header.code;
    sum += ntohs(header.identifier);
    sum += ntohs(header.sequence_number);
    while (size) {
        uint16_t cur = *data++ << 8;
        --size;
        if (size) {
            cur |= *data++;
            --size;
        }
        sum += cur;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    header.checksum = htons(static_cast<uint16_t>(~sum));
}

}  // namespace

class IcmpClient::Operation : public boost::intrusive_ref_counter<
    Operation, boost::thread_unsafe_counter> {
public:
    Operation(
        IcmpClient &client,
        const icmp::endpoint &endpoint,
        uint16_t sequence_number,
        std::vector<uint8_t> request_buffer,
        std::function<void(std::error_code, std::vector<uint8_t>)> callback);
    Operation(const Operation &) = delete;
    Operation &operator=(const Operation &) = delete;
    ~Operation();

    void send();
    void complete(std::error_code ec, std::vector<uint8_t> buffer);

    TimerList::Handle &timer_handle() { return timer_handle_; }

private:
    IcmpClient &client_;
    icmp::endpoint endpoint_;
    uint16_t sequence_number_;
    std::vector<uint8_t> request_buffer_;
    std::function<void(std::error_code, std::vector<uint8_t>)> callback_;
    TimerList::Handle timer_handle_;
    IcmpHeader request_header_;
};

IcmpClient::IcmpClient(const any_io_executor &executor, const Options &options)
    : options_(options),
      // TODO(iceboy): Support IPv6.
      socket_(executor, {icmp::v4(), 0}),
      timer_(executor, options_.timeout),
      identifier_(static_cast<uint16_t>(boost::this_process::get_id())),
      receive_buffer_(
          std::make_unique<uint8_t[]>(options_.receive_buffer_size)) {
    receive();
}

void IcmpClient::request(
    const icmp::endpoint &endpoint,
    std::vector<uint8_t> buffer,
    std::function<void(std::error_code, std::vector<uint8_t>)> callback) {
    uint16_t sequence_number = static_cast<uint16_t>(next_sequence_number_);
    if (operations_.find(sequence_number) != operations_.end()) {
        callback(make_error_code(std::errc::no_buffer_space), {});
        return;
    }
    ++next_sequence_number_;
    boost::intrusive_ptr<Operation> operation(new Operation(
        *this, endpoint, sequence_number, std::move(buffer),
        std::move(callback)));
    operation->send();
    operation->timer_handle() = timer_.schedule([this, operation]() {
        operation->timer_handle() = timer_.null_handle();
        operation->complete(make_error_code(std::errc::timed_out), {});
    });
}

void IcmpClient::receive() {
    socket_.async_receive(
        buffer(receive_buffer_.get(), options_.receive_buffer_size),
        [this](std::error_code ec, size_t size) {
            if (ec) {
                if (ec == std::errc::operation_canceled) {
                    return;
                }
                receive();
                return;
            }
            if (size < 20 + sizeof(IcmpHeader)) {
                receive();
                return;
            }
            IcmpHeader *header = reinterpret_cast<IcmpHeader *>(
                &receive_buffer_[20]);
            if (header->type != echo_reply ||
                ntohs(header->identifier) !=
                    static_cast<uint16_t>(identifier_)) {
                receive();
                return;
            }
            auto iter = operations_.find(ntohs(header->sequence_number));
            if (iter == operations_.end()) {
                receive();
                return;
            }
            uint8_t *data = &receive_buffer_[20 + sizeof(IcmpHeader)];
            size_t data_size = size - (20 + sizeof(IcmpHeader));
            iter->second->complete({}, {data, data + data_size});
            receive();
        });
}

IcmpClient::Operation::Operation(
    IcmpClient &client,
    const icmp::endpoint &endpoint,
    uint16_t sequence_number,
    std::vector<uint8_t> request_buffer,
    std::function<void(std::error_code, std::vector<uint8_t>)> callback)
    : client_(client),
      endpoint_(endpoint),
      sequence_number_(sequence_number),
      request_buffer_(std::move(request_buffer)),
      callback_(std::move(callback)) {
    client_.operations_.emplace(sequence_number_, this);
    request_header_.type = echo_request;
    request_header_.code = 0;
    request_header_.identifier = htons(client_.identifier_);
    request_header_.sequence_number = htons(sequence_number_);
    compute_checksum(
        request_buffer_.data(), request_buffer_.size(), request_header_);
}

IcmpClient::Operation::~Operation() {
    client_.operations_.erase(sequence_number_);
}

void IcmpClient::Operation::send() {
    client_.socket_.async_send_to(
        std::array<const_buffer, 2>{{
            buffer(&request_header_, sizeof(IcmpHeader)),
            buffer(request_buffer_),
        }},
        endpoint_,
        [_ = boost::intrusive_ptr<Operation>(this)](
            std::error_code, size_t) {});
}

void IcmpClient::Operation::complete(
    std::error_code ec, std::vector<uint8_t> buffer) {
    if (callback_) {
        callback_(ec, std::move(buffer));
        callback_ = {};
    }
    if (timer_handle_ != client_.timer_.null_handle()) {
        TimerList::Handle saved_timer_handle = timer_handle_;
        timer_handle_ = client_.timer_.null_handle();
        client_.timer_.cancel(saved_timer_handle);
        // Operation may be freed at this point.
    }
}

}  // namespace net
