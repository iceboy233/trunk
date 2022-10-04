package(default_visibility = ["//visibility:public"])

cc_library(
    name = "asio",
    hdrs = ["asio.h"],
    deps = ["@org_boost_boost//:asio"],
)

cc_test(
    name = "asio_test",
    srcs = ["asio_test.cc"],
    deps = [
        ":asio",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "asio-flags",
    srcs = ["asio-flags.cc"],
    hdrs = ["asio-flags.h"],
    deps = [
        "@org_boost_boost//:asio",
        "@org_boost_boost//:system",
    ],
)

cc_test(
    name = "asio-flags_test",
    srcs = ["asio-flags_test.cc"],
    deps = [
        ":asio-flags",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "asio-hash",
    hdrs = ["asio-hash.h"],
    deps = ["@org_boost_boost//:asio"],
)

cc_binary(
    name = "asio-hash_benchmark",
    srcs = ["asio-hash_benchmark.cc"],
    deps = [
        ":asio",
        ":asio-hash",
        "@com_github_google_benchmark//:benchmark_main",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/random",
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
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "endpoint",
    hdrs = ["endpoint.h"],
    deps = [
        "//net:asio",
        "@com_google_absl//absl/strings",
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
        "@com_google_absl//absl/container:flat_hash_map",
        "@org_boost_boost//:endian",
        "@org_boost_boost//:process",
        "@org_boost_boost//:smart_ptr",
    ],
)

cc_library(
    name = "ip-encoding",
    srcs = ["ip-encoding.cc"],
    hdrs = ["ip-encoding.h"],
    deps = [
        ":asio",
        "@com_google_absl//absl/types:span",
    ],
)

cc_test(
    name = "ip-encoding_test",
    srcs = ["ip-encoding_test.cc"],
    deps = [
        ":asio",
        ":ip-encoding",
        "@com_google_googletest//:gtest_main",
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
        "@com_google_googletest//:gtest_main",
    ],
)
