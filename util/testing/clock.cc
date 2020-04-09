#include "util/testing/clock.h"

namespace util {

MockClock::time_point MockClock::now_ = MockClock::time_point::min();

}  // namespace util
