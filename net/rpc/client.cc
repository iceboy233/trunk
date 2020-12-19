#include "net/rpc/client.h"

#include <limits>
#include <utility>
#include <boost/icl/interval_set.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include "absl/types/span.h"
#include "util/fibonacci-sequence.h"

namespace net {
namespace rpc {

class Client::Operation : public boost::intrusive_ref_counter<
    Operation, boost::thread_unsafe_counter> {
public:
    Operation(
        Client &client,
        const udp::endpoint &endpoint,
        std::vector<uint8_t> request,
        const RequestOptions &options,
        std::function<void(std::error_code, std::vector<uint8_t>)> callback);
    Operation(const Operation &) = delete;
    Operation &operator=(const Operation &) = delete;
    ~Operation();

    void send();
    void unpack(absl::Span<uint8_t> fragment);
    void abort(std::error_code ec);

private:
    Client &client_;
    steady_timer wait_timer_;
    udp::endpoint endpoint_;
    std::vector<uint8_t> request_;
    RequestOptions options_;
    std::function<void(std::error_code, std::vector<uint8_t>)> callback_;
    uint64_t request_id_;
    std::vector<std::vector<uint8_t>> request_buffers_;
    util::FibonacciSequence<int> wait_sequence_;
    bool finished_ = false;
    std::error_code result_ec_ = make_error_code(std::errc::bad_address);
    std::vector<uint8_t> response_;
    boost::icl::interval_set<size_t> response_intervals_;
    size_t response_size_ = std::numeric_limits<size_t>::max();
};

Client::Client(const any_io_executor &executor, const Options &options)
    : executor_(executor),
      options_(options) {
    channels_.reserve(options_.num_channels);
    for (int index = 0; index < options_.num_channels; ++index) {
        // TODO(iceboy): Support IPv6.
        channels_.push_back({
            udp::socket(executor_, {udp::v4(), 0}),
            std::make_unique<uint8_t[]>(options_.receive_buffer_size),
        });
    }
    channels_iter_ = channels_.begin();
    for (Channel &channel : channels_) {
        receive(channel);
    }
}

void Client::request(
    const udp::endpoint &endpoint,
    std::string_view method,
    std::vector<uint8_t> request,
    const RequestOptions &options,
    std::function<void(std::error_code, std::vector<uint8_t>)> callback) {
    if (!request.empty()) {
        request.push_back(0);
    }
    request.insert(request.end(), method.begin(), method.end());
    boost::intrusive_ptr<Operation>(new Operation(
        *this, endpoint, std::move(request), options,
        std::move(callback)))->send();
}

void Client::receive(Channel &channel) {
    channel.socket.async_receive(
        buffer(channel.receive_buffer.get(), options_.receive_buffer_size),
        [this, &channel](std::error_code ec, size_t size) {
            if (ec) {
                if (ec == std::errc::operation_canceled) {
                    return;
                }
                receive(channel);
                return;
            }
            channel.receive_size = size;
            dispatch(channel);
        });
}

void Client::dispatch(Channel &channel) {
    if (channel.receive_size < sizeof(wire::response::Header)) {
        receive(channel);
        return;
    }
    const auto *response_header = reinterpret_cast<wire::response::Header *>(
        &channel.receive_buffer[0]);
    auto iter = operations_.find(
        {response_header->key_fingerprint, response_header->request_id});
    if (iter == operations_.end()) {
        receive(channel);
        return;
    }
    iter->second->unpack({channel.receive_buffer.get(), channel.receive_size});
    receive(channel);
}

Client::Operation::Operation(
    Client &client,
    const udp::endpoint &endpoint,
    std::vector<uint8_t> request,
    const RequestOptions &options,
    std::function<void(std::error_code, std::vector<uint8_t>)> callback)
    : client_(client),
      wait_timer_(client_.executor_),
      endpoint_(endpoint),
      request_(std::move(request)),
      options_(options),
      callback_(std::move(callback)) {
    uint64_t key_fingerprint = options_.key.fingerprint();
    do {
        request_id_ = absl::Uniform<uint64_t>(client_.gen_);
    } while (!client_.operations_.emplace(
        OperationKey{key_fingerprint, request_id_}, this).second);
    security::NonceArray nonce{};
    *reinterpret_cast<uint64_t *>(&nonce[0]) = request_id_;
    size_t offset = 0;
    do {
        size_t size = std::min(request_.size() - offset,
                               client_.options_.send_fragment_size);
        request_buffers_.emplace_back(sizeof(wire::request::Header) + size +
                                      security::Key::encrypt_overhead);
        auto *request_header = reinterpret_cast<wire::request::Header *>(
            &request_buffers_.back()[0]);
        request_header->key_fingerprint = key_fingerprint;
        request_header->request_id = request_id_;
        request_header->offset = static_cast<uint16_t>(offset);
        request_header->flags = 0;
        if (offset + size < request_.size()) {
            request_header->flags |= wire::request::flags::partial;
        }
        *reinterpret_cast<uint16_t *>(&nonce[8]) =
            static_cast<uint16_t>(offset);
        size_t encrypt_result = options_.key.encrypt(
            {&request_[offset], size},
            &nonce[0],
            &request_buffers_.back()[sizeof(wire::request::Header)]);
        request_buffers_.back().resize(
            sizeof(wire::request::Header) + encrypt_result);
        offset += size;
    } while (offset < request_.size());
}

Client::Operation::~Operation() {
    client_.operations_.erase({options_.key.fingerprint(), request_id_});
    if (result_ec_) {
        callback_(result_ec_, {});
        return;
    }
    response_.resize(response_size_);
    callback_({}, std::move(response_));
}

void Client::Operation::send() {
    if (options_.timeout <= std::chrono::nanoseconds::zero()) {
        abort(make_error_code(std::errc::timed_out));
        return;
    }
    Channel &channel = *client_.channels_iter_++;
    if (client_.channels_iter_ == client_.channels_.end()) {
        client_.channels_iter_ = client_.channels_.begin();
    }
    for (const std::vector<uint8_t> &request_buffer : request_buffers_) {
        channel.socket.async_send_to(
            buffer(request_buffer),
            endpoint_,
            [_ = boost::intrusive_ptr<Operation>(this)](
                std::error_code, size_t) {});
    }
    std::chrono::nanoseconds wait_period =
        std::min(options_.initial_retry * wait_sequence_.a(), options_.timeout);
    wait_sequence_.next();
    options_.timeout -= wait_period;
    wait_timer_.expires_after(wait_period);
    wait_timer_.async_wait(
        [this, _ = boost::intrusive_ptr<Operation>(this)](std::error_code ec) {
            if (ec) {
                if (ec != std::errc::operation_canceled) {
                    abort(ec);
                }
                return;
            }
            send();
        });
}

void Client::Operation::unpack(absl::Span<uint8_t> fragment) {
    if (finished_) {
        return;
    }
    const auto *response_header = reinterpret_cast<wire::response::Header *>(
        &fragment[0]);
    size_t offset = response_header->offset;
    size_t response_body_size =
        fragment.size() - sizeof(wire::response::Header);
    if (response_.size() < offset + response_body_size) {
        response_.resize(offset + response_body_size);
    }
    security::NonceArray nonce{};
    *reinterpret_cast<uint64_t *>(&nonce[0]) = response_header->request_id;
    *reinterpret_cast<uint16_t *>(&nonce[8]) = response_header->offset;
    nonce[11] = 1;
    size_t decrypt_result = options_.key.decrypt(
        {&fragment[sizeof(wire::response::Header)], response_body_size},
        &nonce[0],
        &response_[offset]);
    if (decrypt_result == std::numeric_limits<size_t>::max()) {
        return;
    }
    response_intervals_ += boost::icl::interval<size_t>::right_open(
        offset, offset + decrypt_result);
    if (!(response_header->flags & wire::response::flags::partial)) {
        response_size_ = offset + decrypt_result;
    }
    if (contains(response_intervals_,
                 boost::icl::interval<size_t>::right_open(0, response_size_))) {
        finished_ = true;
        result_ec_ = {};
        wait_timer_.cancel();
    }
}

void Client::Operation::abort(std::error_code ec) {
    if (finished_) {
        return;
    }
    finished_ = true;
    result_ec_ = ec;
    wait_timer_.cancel();
}

}  // namespace rpc
}  // namespace net
