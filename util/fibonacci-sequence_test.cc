#include "util/fibonacci-sequence.h"

#include <gtest/gtest.h>

namespace util {
namespace {

TEST(FibonacciSequenceTest, Values) {
    FibonacciSequence<int> fib;
    EXPECT_EQ(fib.a(), 1);
    fib.next();
    EXPECT_EQ(fib.a(), 1);
    fib.next();
    EXPECT_EQ(fib.a(), 2);
    fib.next();
    EXPECT_EQ(fib.a(), 3);
    fib.next();
    EXPECT_EQ(fib.a(), 5);
    fib.next();
    EXPECT_EQ(fib.a(), 8);
    fib.next();
    EXPECT_EQ(fib.a(), 13);
}

}  // namespace
}  // namespace util
