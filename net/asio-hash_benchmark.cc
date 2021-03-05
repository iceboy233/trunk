#include <cstdint>
#include <benchmark/benchmark.h>
#include "absl/container/flat_hash_map.h"
#include "absl/random/random.h"
#include "net/asio.h"
#include "net/asio-hash.h"

namespace net {
namespace {

void BM_lookup(benchmark::State &state) {
    absl::BitGen gen;
    absl::flat_hash_map<udp::endpoint, uint64_t> map;
    std::vector<udp::endpoint> keys;
    for (int i = 0; i < state.range(0); ++i) {
        udp::endpoint key(
            address_v4(absl::Uniform<uint32_t>(gen)),
            absl::Uniform<uint16_t>(gen));
        map.emplace(key, absl::Uniform<uint64_t>(gen));
        keys.push_back(key);
    }
    auto iter = keys.begin();
    for (auto _ : state) {
        if (iter == keys.end()) {
            iter = keys.begin();
        }
        benchmark::DoNotOptimize(map.find(*iter++)->second);
    }
}

BENCHMARK(BM_lookup)->Range(1, 1048576);

}  // namespace
}  // namespace net
