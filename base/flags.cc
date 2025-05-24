#include "base/flags.h"

#include <vector>

#include "absl/base/no_destructor.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"

namespace base {
namespace detail {
namespace {

absl::NoDestructor<std::vector<void (*)()>> setters;

}  // namespace

void register_flag(void (*setter)()) {
    setters->push_back(setter);
}

}  // namespace detail

void parse_flags(int argc, char *argv[]) {
    absl::SetProgramUsageMessage("");
    absl::ParseCommandLine(argc, argv);
    for (const auto &setter : *detail::setters) {
        setter();
    }
}

}  // namespace base
