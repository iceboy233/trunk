#include "io/file-utils.h"

namespace io {

std::error_code read_to_end(File &file, std::string &content) {
    content.clear();
    size_t pos = 0;
    size_t size;
    do {
        content.resize(pos + 8192);
        std::error_code ec = file.read({&content[pos], 8192}, size);
        if (ec) {
            return ec;
        }
        pos += size;
        content.resize(pos);
    } while (size);
    return {};
}

}  // namespace io
