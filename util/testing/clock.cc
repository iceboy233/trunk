#include "util/testing/clock.h"

namespace util {
namespace {

MockClock::time_point mock_time = MockClock::time_point::min();

}  // namespace

MockClock::time_point MockClock::now() {
    return mock_time;
}

void MockClock::reset() {
    mock_time = MockClock::time_point::min();
}

void MockClock::set(time_point time) {
    mock_time = time;
}

void MockClock::advance(duration duration) {
    mock_time += duration;
}

}  // namespace util
