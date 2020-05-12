#include "net/rpc/server.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <system_error>
#include <utility>
#include <boost/icl/interval_set.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include "net/rpc/flatbuffers-handle.h"
#include "net/rpc/flatbuffers/list_generated.h"
#include "net/rpc/wire-structs.h"
#include "security/keys/zero.key.h"

namespace net {
namespace rpc {

class Server::Operation : public boost::intrusive_ref_counter<
    Operation, boost::thread_unsafe_counter> {
public:
    Operation(
        Server &server,
        const udp::endpoint &client_endpoint,
        uint64_t key_fingerprint,
        uint64_t request_id);
    Operation(const Operation &) = delete;
    Operation &operator=(const Operation &) = delete;
    ~Operation();

    void unpack();

    void set_client_endpoint(const udp::endpoint &client_endpoint) {
        client_endpoint_ = client_endpoint;
    }

private:
    void handle();
    void pack();
    void respond();

    Server &server_;
    steady_timer keep_alive_timer_;
    udp::endpoint client_endpoint_;
    uint64_t key_fingerprint_;
    uint64_t request_id_;
    std::optional<security::Key> key_;
    std::vector<uint8_t> request_;
    boost::icl::interval_set<size_t> request_intervals_;
    size_t request_size_ = std::numeric_limits<size_t>::max();
    std::vector<uint8_t> response_;
    std::vector<std::vector<uint8_t>> response_buffers_;
};

Server::Server(
    const executor &executor,
    const udp::endpoint &endpoint,
    const Options &options)
    : executor_(executor),
      options_(options),
      socket_(executor_, endpoint),
      receive_buffer_(
          std::make_unique<uint8_t[]>(options_.receive_buffer_size)) {
    handle("ping", [](
        std::vector<uint8_t> request,
        const security::Key &,
        const auto &callback) {
        callback(std::move(request));
    });
    flatbuffers_handle<fbs::ListRequest, fbs::ListResponse>(
        *this, "list", [this](
            const fbs::ListRequestT &,
            const security::Key &,
            const auto &callback) {
            fbs::ListResponseT response;
            for (const auto &pair : handlers_) {
                response.methods.push_back(pair.first);
            }
            std::sort(response.methods.begin(), response.methods.end());
            callback(std::move(response));
        });
    add_key(security::keys::zero);
}

void Server::handle(std::string_view name, HandlerFunc handler) {
    handlers_.emplace(name, std::move(handler));
}

void Server::handle(std::string_view name, std::unique_ptr<Handler> handler) {
    handle(name, [handler = std::shared_ptr<Handler>(std::move(handler))](
        std::vector<uint8_t> request,
        const security::Key &key,
        std::function<void(std::vector<uint8_t>)> callback) {
        handler->handle(std::move(request), key, std::move(callback));
    });
}

void Server::add_key(const security::Key &key) {
    keystore_.add(key);
}

void Server::receive() {
    socket_.async_receive_from(
        buffer(receive_buffer_.get(), options_.receive_buffer_size),
        receive_endpoint_,
        [this](std::error_code ec, size_t size) {
            if (ec) {
                if (ec == std::errc::connection_refused ||
                    ec == std::errc::connection_reset) {
                    receive();
                }
                return;
            }
            receive_size_ = size;
            dispatch();
        });
}

void Server::dispatch() {
    if (receive_size_ < sizeof(wire::response::Header)) {
        receive();
        return;
    }
    const auto *request_header = reinterpret_cast<wire::request::Header *>(
        &receive_buffer_[0]);
    auto iter = operations_.find(
        {request_header->key_fingerprint, request_header->request_id});
    if (iter != operations_.end()) {
        iter->second->set_client_endpoint(receive_endpoint_);
        iter->second->unpack();
    } else {
        boost::intrusive_ptr<Operation>(new Operation(
            *this,
            receive_endpoint_,
            request_header->key_fingerprint,
            request_header->request_id))->unpack();
    }
    receive();
}

Server::Operation::Operation(
    Server &server,
    const udp::endpoint &client_endpoint,
    uint64_t key_fingerprint,
    uint64_t request_id)
    : server_(server),
      keep_alive_timer_(server_.executor_),
      client_endpoint_(client_endpoint),
      key_fingerprint_(key_fingerprint),
      request_id_(request_id) {
    server_.operations_.emplace(
        OperationKey{key_fingerprint_, request_id_}, this);
}

Server::Operation::~Operation() {
    server_.operations_.erase({key_fingerprint_, request_id_});
}

void Server::Operation::unpack() {
    const auto *request_header = reinterpret_cast<wire::request::Header *>(
        &server_.receive_buffer_[0]);
    size_t offset = request_header->offset;
    size_t request_body_size =
        server_.receive_size_ - sizeof(wire::request::Header);
    if (request_.size() < offset + request_body_size) {
        request_.resize(offset + request_body_size);
    }
    security::NonceArray nonce{};
    *reinterpret_cast<uint64_t *>(&nonce[0]) = request_header->request_id;
    *reinterpret_cast<uint16_t *>(&nonce[8]) = request_header->offset;
    size_t decrypt_result;
    if (key_) {
        decrypt_result = key_->decrypt(
            {
                &server_.receive_buffer_[sizeof(wire::request::Header)],
                request_body_size,
            },
            &nonce[0],
            &request_[offset]);
        if (decrypt_result == std::numeric_limits<size_t>::max()) {
            return;
        }
    } else {
        std::vector<security::Key> keys;
        server_.keystore_.find(request_header->key_fingerprint, keys);
        decrypt_result = std::numeric_limits<size_t>::max();
        for (const security::Key &key : keys) {
            decrypt_result = key.decrypt(
                {
                    &server_.receive_buffer_[sizeof(wire::request::Header)],
                    request_body_size,
                },
                &nonce[0],
                &request_[offset]);
            if (decrypt_result != std::numeric_limits<size_t>::max()) {
                key_.emplace(key);
                break;
            }
        }
        if (decrypt_result == std::numeric_limits<size_t>::max()) {
            return;
        }
        keep_alive_timer_.expires_after(std::chrono::minutes(1));
        keep_alive_timer_.async_wait(
            [_ = boost::intrusive_ptr<Operation>(this)](std::error_code) {});
    }
    if (contains(request_intervals_,
                 boost::icl::interval<size_t>::right_open(0, request_size_))) {
        if (!(request_header->flags & wire::request::flags::partial)) {
            respond();
        }
        return;
    }
    request_intervals_ += boost::icl::interval<size_t>::right_open(
        offset, offset + decrypt_result);
    if (!(request_header->flags & wire::request::flags::partial)) {
        request_size_ = offset + decrypt_result;
    }
    if (contains(request_intervals_,
                 boost::icl::interval<size_t>::right_open(0, request_size_))) {
        handle();
    }
}

void Server::Operation::handle() {
    const uint8_t *request_body_end = &request_[request_size_];
    const uint8_t *request_end = request_body_end;
    while (request_end != &request_[0] && request_end[-1]) {
        --request_end;
    }
    std::string_view method(
        reinterpret_cast<const char *>(request_end),
        request_body_end - request_end);
    if (request_end != &request_[0]) {
        --request_end;
    }
    auto handler_iter = server_.handlers_.find(method);
    if (handler_iter == server_.handlers_.end()) {
        // TODO(iceboy): Invalid request?
        return;
    }
    request_.resize(request_end - &request_[0]);
    handler_iter->second(
        std::move(request_),
        *key_,
        [this, _ = boost::intrusive_ptr<Operation>(this)](
            std::vector<uint8_t> response) {
            response_ = std::move(response);
            pack();
        });
}

void Server::Operation::pack() {
    security::NonceArray nonce{};
    *reinterpret_cast<uint64_t *>(&nonce[0]) = request_id_;
    nonce[11] = 1;
    size_t offset = 0;
    do {
        size_t size = std::min(response_.size() - offset,
                               server_.options_.send_fragment_size);
        response_buffers_.emplace_back(sizeof(wire::response::Header) + size +
                                       security::Key::encrypt_overhead);
        auto *response_header = reinterpret_cast<wire::response::Header *>(
            &response_buffers_.back()[0]);
        response_header->key_fingerprint = key_fingerprint_;
        response_header->request_id = request_id_;
        response_header->offset = static_cast<uint16_t>(offset);
        response_header->flags = 0;
        if (offset + size < response_.size()) {
            response_header->flags |= wire::response::flags::partial;
        }
        *reinterpret_cast<uint16_t *>(&nonce[8]) =
            static_cast<uint16_t>(offset);
        size_t encrypt_result = key_->encrypt(
            {&response_[offset], size},
            &nonce[0],
            &response_buffers_.back()[sizeof(wire::response::Header)]);
        response_buffers_.back().resize(
            sizeof(wire::response::Header) + encrypt_result);
        offset += size;
    } while (offset < response_.size());
    respond();
}

void Server::Operation::respond() {
    for (const std::vector<uint8_t> &response_buffer : response_buffers_) {
        server_.socket_.async_send_to(
            buffer(response_buffer),
            client_endpoint_,
            [_ = boost::intrusive_ptr<Operation>(this)](
                std::error_code, size_t) {});
    }
}

}  // namespace rpc
}  // namespace net
