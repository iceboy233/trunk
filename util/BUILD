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
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "flatbuffers",
    hdrs = ["flatbuffers.h"],
    deps = [
        "@abseil-cpp//absl/types:span",
        "@flatbuffers//:runtime_cc",
    ],
)

cc_test(
    name = "flatbuffers_test",
    srcs = ["flatbuffers_test.cc"],
    deps = [
        ":flatbuffers",
        "//util/flatbuffers:test",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "flatbuffers-reflection",
    srcs = ["flatbuffers-reflection.cc"],
    hdrs = ["flatbuffers-reflection.h"],
    deps = [
        "@abseil-cpp//absl/types:span",
        "@flatbuffers",
    ],
)

cc_test(
    name = "flatbuffers-reflection_test",
    srcs = ["flatbuffers-reflection_test.cc"],
    deps = [
        ":flatbuffers-reflection",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "hash-filter",
    srcs = ["hash-filter.cc"],
    hdrs = ["hash-filter.h"],
    deps = [
        "@abseil-cpp//absl/base",
        "@abseil-cpp//absl/random",
    ],
)

cc_binary(
    name = "hash-filter_benchmark",
    srcs = ["hash-filter_benchmark.cc"],
    deps = [
        ":hash-filter",
        "@abseil-cpp//absl/random",
        "@google_benchmark//:benchmark_main",
    ],
)

cc_binary(
    name = "hash-filter_stats",
    srcs = ["hash-filter_stats.cc"],
    deps = [
        ":hash-filter",
        "//io:native-file",
        "//io:stream",
        "@abseil-cpp//absl/random",
    ],
)

cc_test(
    name = "hash-filter_test",
    srcs = ["hash-filter_test.cc"],
    deps = [
        ":hash-filter",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "hex-dumper",
    srcs = ["hex-dumper.cc"],
    hdrs = ["hex-dumper.h"],
    deps = [
        "//base:types",
        "//io:file",
        "//io:file-utils",
        "@abseil-cpp//absl/container:fixed_array",
    ],
)

cc_test(
    name = "hex-dumper_test",
    srcs = ["hex-dumper_test.cc"],
    deps = [
        ":hex-dumper",
        "//base:types",
        "//io:memory-file",
        "@googletest//:gtest_main",
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
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "rope",
    srcs = ["rope.cc"],
    hdrs = ["rope.h"],
    deps = ["@abseil-cpp//absl/random"],
)

cc_test(
    name = "rope_test",
    srcs = ["rope_test.cc"],
    deps = [
        ":rope",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "strings",
    srcs = ["strings.cc"],
    hdrs = ["strings.h"],
)

cc_test(
    name = "strings_test",
    srcs = ["strings_test.cc"],
    deps = [
        ":strings",
        "@googletest//:gtest_main",
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
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "varint",
    hdrs = ["varint.h"],
    deps = ["@abseil-cpp//absl/types:span"],
)

cc_test(
    name = "varint_test",
    srcs = ["varint_test.cc"],
    deps = [
        ":varint",
        "@abseil-cpp//absl/types:span",
        "@googletest//:gtest_main",
    ],
)
