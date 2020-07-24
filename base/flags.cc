#include "base/flags.h"

#include <utility>
#include <vector>
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"

namespace base {
namespace detail {
namespace {

std::vector<std::function<void()>> &setters() {
    // TODO(iceboy): Remove heap allocation.
    static auto *setters = new std::vector<std::function<void()>>;
    return *setters;
}

}  // namespace

void register_flag(std::function<void()> setter) {
    setters().push_back(std::move(setter));
}

}  // namespace detail

void parse_flags(int argc, char *argv[]) {
    absl::SetProgramUsageMessage("");
    absl::ParseCommandLine(argc, argv);
    for (const auto &setter : detail::setters()) {
        setter();
    }
}

}  // namespace base
