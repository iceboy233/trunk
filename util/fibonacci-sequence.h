#ifndef _UTIL_FIBONACCI_SEQUENCE_H
#define _UTIL_FIBONACCI_SEQUENCE_H

namespace util {

template <typename T>
class FibonacciSequence {
public:
    FibonacciSequence() { reset(); }
    void reset() { a_ = 1; b_ = 1; }
    T a() const { return a_; }
    T b() const { return b_; }
    void next() { T a = a_; a_ = b_; b_ += a; }

private:
    T a_;
    T b_;
};

}  // namespace util

#endif  // _UTIL_FIBONACCI_SEQUENCE_H
