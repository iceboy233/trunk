#include <chrono>

namespace util {

class MockClock {
public:
    using duration = std::chrono::nanoseconds;
    using time_point = std::chrono::time_point<MockClock, duration>;

    static time_point now();
    static void reset();
    static void set(time_point time);
    static void advance(duration duration);
};

}  // namespace util
