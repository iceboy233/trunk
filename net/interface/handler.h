#ifndef _NET_INTERFACE_HANDLER_H
#define _NET_INTERFACE_HANDLER_H

#include <memory>

#include "net/interface/datagram.h"
#include "net/interface/stream.h"

namespace net {

class Handler {
public:
    virtual ~Handler() = default;

    virtual void handle_stream(std::unique_ptr<Stream> stream) = 0;
    virtual void handle_datagram(std::unique_ptr<Datagram> datagram) = 0;
};

}  // namespace net

#endif  // _NET_INTERFACE_HANDLER_H
