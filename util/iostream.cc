#include "util/iostream.h"

#include <sstream>

namespace util {

bool read(std::istream &in, std::string &out) {
    std::ostringstream copy;
    copy << in.rdbuf();
    if (in.fail()) {
        return false;
    }
    out = copy.str();
    return true;
}

bool write(std::string_view in, std::ostream &out) {
    out.write(in.data(), in.size());
    return !out.fail();
}

}  // namespace util
