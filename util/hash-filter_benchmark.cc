#include <cstdint>

#include "absl/random/random.h"
#include "benchmark/benchmark.h"
#include "util/hash-filter.h"

namespace util {
namespace {

void BM_HashFilter32_insert(benchmark::State &state) {
    absl::InsecureBitGen gen;
    HashFilter32 filter(262144);
    for (auto _ : state) {
        if (filter.size() >= state.range(0)) {
            filter.clear();
        }
        filter.insert(absl::Uniform<uint64_t>(gen));
    }
}

void BM_HashFilter32_test(benchmark::State &state) {
    absl::InsecureBitGen gen;
    HashFilter32 filter(262144);
    while (filter.size() < state.range(0)) {
        filter.insert(absl::Uniform<uint64_t>(gen));
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(filter.test(absl::Uniform<uint64_t>(gen)));
    }
}

BENCHMARK(BM_HashFilter32_insert)->Range(32768, 1000000);
BENCHMARK(BM_HashFilter32_test)->Range(32768, 1000000);

}  // namespace
}  // namespace util
