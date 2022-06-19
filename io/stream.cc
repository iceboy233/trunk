#include "io/stream.h"

#include "io/file-utils.h"

namespace io {

OStream::OStream(File &file)
    : std::ostream(this),
      file_(file) {
    setp(buffer_.begin(), buffer_.end());
}

int OStream::sync() {
    size_t size = pptr() - pbase();
    if (!size) {
        return 0;
    }
    if (io::write(file_, {pbase(), size})) {
        return -1;
    }
    setp(buffer_.begin(), buffer_.end());
    return 0;
}

int OStream::overflow(int c) {
    if (sync() < 0) {
        return EOF;
    }
    if (c != EOF) {
        *pptr() = c;
        pbump(1);
    }
    return 0;
}

}  // namespace io
