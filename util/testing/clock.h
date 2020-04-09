#include <chrono>

namespace util {

class MockClock {
public:
    using duration = std::chrono::nanoseconds;
    using time_point = std::chrono::time_point<MockClock, duration>;

    static void reset() { now_ = time_point::min(); }
    static time_point now() { return now_; }
    static void set_now(time_point now) { now_ = now; }
    static void advance(duration duration) { now_ += duration; }

private:
    static time_point now_;
};

}  // namespace util
