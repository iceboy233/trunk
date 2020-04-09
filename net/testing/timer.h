#ifndef _NET_TESTING_TIMER_H
#define _NET_TESTING_TIMER_H

#include <chrono>

namespace net {

template <typename TimePointT>
class BasicPollingWaitTraits {
public:
    static typename TimePointT::duration to_wait_duration(
        typename TimePointT::duration);
    static typename TimePointT::duration to_wait_duration(TimePointT);
};

template <typename TimePointT>
typename TimePointT::duration
BasicPollingWaitTraits<TimePointT>::to_wait_duration(
    typename TimePointT::duration) {
    return TimePointT::duration::zero();
}

template <typename TimePointT>
typename TimePointT::duration
BasicPollingWaitTraits<TimePointT>::to_wait_duration(TimePointT) {
    return TimePointT::duration::zero();
}

using PollingWaitTraits = BasicPollingWaitTraits<
    std::chrono::steady_clock::time_point>;

}  // namespace net

#endif  // _NET_TESTING_TIMER_H
