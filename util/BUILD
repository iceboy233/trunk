package(default_visibility = ["//visibility:public"])

cc_library(
    name = "fibonacci-sequence",
    hdrs = ["fibonacci-sequence.h"],
)

cc_test(
    name = "fibonacci-sequence_test",
    srcs = ["fibonacci-sequence_test.cc"],
    deps = [
        ":fibonacci-sequence",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "flatbuffers",
    hdrs = ["flatbuffers.h"],
    deps = [
        "@com_github_google_flatbuffers//:runtime_cc",
        "@com_google_absl//absl/types:span",
    ],
)

cc_test(
    name = "flatbuffers_test",
    srcs = ["flatbuffers_test.cc"],
    deps = [
        ":flatbuffers",
        "//util/flatbuffers:test",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "flatbuffers-reflection",
    srcs = ["flatbuffers-reflection.cc"],
    hdrs = ["flatbuffers-reflection.h"],
    deps = [
        "@com_github_google_flatbuffers//:flatbuffers",
        "@com_google_absl//absl/types:span",
    ],
)

cc_test(
    name = "flatbuffers-reflection_test",
    srcs = ["flatbuffers-reflection_test.cc"],
    deps = [
        ":flatbuffers-reflection",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "hash-filter",
    srcs = ["hash-filter.cc"],
    hdrs = ["hash-filter.h"],
    deps = [
        "@com_google_absl//absl/base",
        "@com_google_absl//absl/random",
    ],
)

cc_binary(
    name = "hash-filter_benchmark",
    srcs = ["hash-filter_benchmark.cc"],
    deps = [
        ":hash-filter",
        "@com_google_absl//absl/random",
        "@com_github_google_benchmark//:benchmark_main",
    ],
)

cc_binary(
    name = "hash-filter_stats",
    srcs = ["hash-filter_stats.cc"],
    deps = [
        ":hash-filter",
        "@com_google_absl//absl/random",
    ],
)

cc_test(
    name = "hash-filter_test",
    srcs = ["hash-filter_test.cc"],
    deps = [
        ":hash-filter",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "iostream",
    srcs = ["iostream.cc"],
    hdrs = ["iostream.h"],
)

cc_test(
    name = "iostream_test",
    srcs = ["iostream_test.cc"],
    deps = [
        ":iostream",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "rope",
    srcs = ["rope.cc"],
    hdrs = ["rope.h"],
    deps = ["@com_google_absl//absl/random"],
)

cc_test(
    name = "rope_test",
    srcs = ["rope_test.cc"],
    deps = [
        ":rope",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "pod",
    hdrs = ["pod.h"],
)

cc_test(
    name = "pod_test",
    srcs = ["pod_test.cc"],
    deps = [
        ":pod",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "utf",
    srcs = ["utf.cc"],
    hdrs = ["utf.h"],
)

cc_test(
    name = "utf_test",
    srcs = ["utf_test.cc"],
    deps = [
        ":utf",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "varint",
    hdrs = ["varint.h"],
    deps = ["@com_google_absl//absl/types:span"],
)

cc_test(
    name = "varint_test",
    srcs = ["varint_test.cc"],
    deps = [
        ":varint",
        "@com_google_absl//absl/types:span",
        "@com_google_googletest//:gtest_main",
    ],
)
