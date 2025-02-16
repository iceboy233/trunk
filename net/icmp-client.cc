#include "net/icmp-client.h"

#include <boost/endian/buffers.hpp>
#include <boost/process/environment.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

namespace net {
namespace {

struct IcmpHeader {
    uint8_t type;
    uint8_t code;
    boost::endian::big_uint16_buf_t checksum;
    boost::endian::big_uint16_buf_t identifier;
    boost::endian::big_uint16_buf_t sequence_number;
};
static_assert(sizeof(IcmpHeader) == 8, "");

constexpr uint8_t echo_reply = 0;
constexpr uint8_t echo_request = 8;

void compute_checksum(ConstBufferSpan buffer, IcmpHeader &header) {
    uint32_t sum = (header.type << 8) | header.code;
    sum += header.identifier.value();
    sum += header.sequence_number.value();
    while (buffer.size() >= 2) {
        sum += (buffer[0] << 8) | buffer[1];
        buffer.remove_prefix(2);
    }
    if (!buffer.empty()) {
        sum += buffer[0] << 8;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    header.checksum = static_cast<uint16_t>(~sum);
}

}  // namespace

class IcmpClient::Operation : public boost::intrusive_ref_counter<
    Operation, boost::thread_unsafe_counter> {
public:
    Operation(
        IcmpClient &client,
        const icmp::endpoint &endpoint,
        uint16_t sequence_number,
        absl::AnyInvocable<void(std::error_code, ConstBufferSpan) &&> callback);
    ~Operation();

    Operation(const Operation &) = delete;
    Operation &operator=(const Operation &) = delete;

    void start(ConstBufferSpan buffer);
    void finish(std::error_code ec, ConstBufferSpan buffer);

private:
    IcmpClient &client_;
    icmp::endpoint endpoint_;
    uint16_t sequence_number_;
    absl::AnyInvocable<void(std::error_code, ConstBufferSpan) &&> callback_;
    std::optional<TimerList::Timer> timer_;
    IcmpHeader request_header_;
};

IcmpClient::IcmpClient(const any_io_executor &executor, const Options &options)
    // TODO(iceboy): Support IPv6.
    : socket_(executor, {icmp::v4(), 0}),
      timer_list_(executor, options.timeout),
      identifier_(static_cast<uint16_t>(boost::this_process::get_id())),
      receive_buffer_(std::make_unique<uint8_t[]>(options.receive_buffer_size)),
      receive_buffer_size_(options.receive_buffer_size) {
    receive();
}

void IcmpClient::request(
    const icmp::endpoint &endpoint,
    ConstBufferSpan buffer,
    absl::AnyInvocable<void(std::error_code, ConstBufferSpan) &&> callback) {
    uint16_t sequence_number = next_sequence_number_;
    if (operations_.find(sequence_number) != operations_.end()) {
        std::move(callback)(make_error_code(std::errc::no_buffer_space), {});
        return;
    }
    ++next_sequence_number_;
    boost::intrusive_ptr<Operation> operation(new Operation(
        *this, endpoint, sequence_number, std::move(callback)));
    operation->start(buffer);
}

void IcmpClient::receive() {
    socket_.async_receive(
        buffer(receive_buffer_.get(), receive_buffer_size_),
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
            auto *header = reinterpret_cast<IcmpHeader *>(&receive_buffer_[20]);
            if (header->type != echo_reply ||
                header->identifier.value() != identifier_) {
                receive();
                return;
            }
            auto iter = operations_.find(header->sequence_number.value());
            if (iter == operations_.end()) {
                receive();
                return;
            }
            uint8_t *data = &receive_buffer_[20 + sizeof(IcmpHeader)];
            size_t data_size = size - (20 + sizeof(IcmpHeader));
            iter->second->finish({}, {data, data_size});
            receive();
        });
}

IcmpClient::Operation::Operation(
    IcmpClient &client,
    const icmp::endpoint &endpoint,
    uint16_t sequence_number,
    absl::AnyInvocable<void(std::error_code, ConstBufferSpan) &&> callback)
    : client_(client),
      endpoint_(endpoint),
      sequence_number_(sequence_number),
      callback_(std::move(callback)) {
    client_.operations_.emplace(sequence_number_, this);
}

IcmpClient::Operation::~Operation() {
    client_.operations_.erase(sequence_number_);
}

void IcmpClient::Operation::start(ConstBufferSpan buffer) {
    request_header_.type = echo_request;
    request_header_.code = 0;
    request_header_.identifier = client_.identifier_;
    request_header_.sequence_number = sequence_number_;
    compute_checksum(buffer, request_header_);
    client_.socket_.async_send_to(
        std::array<const_buffer, 2>{{
            net::buffer(&request_header_, sizeof(IcmpHeader)),
            net::buffer(buffer.data(), buffer.size()),
        }},
        endpoint_,
        [_ = boost::intrusive_ptr<Operation>(this)](
            std::error_code, size_t) {});
    timer_.emplace(
        client_.timer_list_,
        [this]() { finish(make_error_code(std::errc::timed_out), {}); });
    intrusive_ptr_add_ref(this);
}

void IcmpClient::Operation::finish(std::error_code ec, ConstBufferSpan buffer) {
    std::move(callback_)(ec, buffer);
    intrusive_ptr_release(this);
}

}  // namespace net
