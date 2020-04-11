#ifndef _SECURITY_TYPES_H
#define _SECURITY_TYPES_H

#include <array>

namespace security {

using KeyArray = std::array<uint8_t, 32>;
using NonceArray = std::array<uint8_t, 12>;

}

#endif  // _SECURITY_TYPES_H
