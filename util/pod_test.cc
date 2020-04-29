#include "util/pod.h"

#include <gtest/gtest.h>

namespace util {
namespace {

TEST(PodTest, create_destroy) {
    enum class State {
        init,
        create,
        destroy,
    };

    class Mutation {
    public:
        Mutation(State &state) : state_(state) { state_ = State::create; }
        ~Mutation() { state_ = State::destroy; }

    private:
        State &state_;
    };

    State state = State::init;
    Pod<Mutation> mutation;
    EXPECT_EQ(state, State::init);
    mutation.emplace(state);
    EXPECT_EQ(state, State::create);
    mutation.destroy();
    EXPECT_EQ(state, State::destroy);
}

}  // namespace
}  // namespace util
