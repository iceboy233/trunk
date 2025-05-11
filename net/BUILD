package(default_visibility = ["//visibility:public"])

cc_library(
    name = "asio",
    hdrs = ["asio.h"],
    deps = ["@boost.asio"],
)

cc_test(
    name = "asio_test",
    srcs = ["asio_test.cc"],
    deps = [
        ":asio",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "asio-flags",
    srcs = ["asio-flags.cc"],
    hdrs = ["asio-flags.h"],
    deps = [
        "@boost.asio",
        "@boost.system",
    ],
)

cc_test(
    name = "asio-flags_test",
    srcs = ["asio-flags_test.cc"],
    deps = [
        ":asio-flags",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "asio-hash",
    hdrs = ["asio-hash.h"],
    deps = ["@boost.asio"],
)

cc_binary(
    name = "asio-hash_benchmark",
    srcs = ["asio-hash_benchmark.cc"],
    deps = [
        ":asio",
        ":asio-hash",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/random",
        "@google_benchmark//:benchmark_main",
    ],
)

cc_library(
    name = "blocking-result",
    hdrs = ["blocking-result.h"],
    deps = [":asio"],
)

cc_test(
    name = "blocking-result_test",
    srcs = ["blocking-result_test.cc"],
    deps = [
        ":blocking-result",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "icmp-client",
    srcs = ["icmp-client.cc"],
    hdrs = ["icmp-client.h"],
    deps = [
        ":asio",
        ":timer-list",
        "//base:types",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/functional:any_invocable",
        "@boost.endian",
        "@boost.process",
        "@boost.smart_ptr",
    ],
)

cc_library(
    name = "ip-encoding",
    srcs = ["ip-encoding.cc"],
    hdrs = ["ip-encoding.h"],
    deps = [
        ":asio",
        "@abseil-cpp//absl/types:span",
    ],
)

cc_test(
    name = "ip-encoding_test",
    srcs = ["ip-encoding_test.cc"],
    deps = [
        ":asio",
        ":ip-encoding",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "rate-limiter",
    srcs = ["rate-limiter.cc"],
    hdrs = ["rate-limiter.h"],
    deps = [":asio"],
)

cc_library(
    name = "timer-list",
    hdrs = ["timer-list.h"],
    deps = [":asio"],
)

cc_test(
    name = "timer-list_test",
    srcs = ["timer-list_test.cc"],
    deps = [
        ":asio",
        ":timer-list",
        "//net/testing:timer",
        "//util/testing:clock",
        "@googletest//:gtest_main",
    ],
)
