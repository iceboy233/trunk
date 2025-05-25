package(default_visibility = ["//visibility:public"])

cc_library(
    name = "file",
    hdrs = ["file.h"],
    deps = ["//base:types"],
)

cc_library(
    name = "file-utils",
    srcs = ["file-utils.cc"],
    hdrs = ["file-utils.h"],
    deps = [":file"],
)

cc_test(
    name = "file-utils_test",
    srcs = ["file-utils_test.cc"],
    deps = [
        ":file-utils",
        ":memory-file",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "line-reader",
    srcs = ["line-reader.cc"],
    hdrs = ["line-reader.h"],
    deps = [
        ":file",
        "@abseil-cpp//absl/types:span",
    ],
)

cc_test(
    name = "line-reader_test",
    srcs = ["line-reader_test.cc"],
    deps = [
        ":line-reader",
        ":memory-file",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "memory-file",
    srcs = ["memory-file.cc"],
    hdrs = ["memory-file.h"],
    deps = [":file"],
)

cc_library(
    name = "native-file",
    srcs = ["native-file.cc"],
    hdrs = ["native-file.h"],
    deps = select({
        "@platforms//os:windows": [
            "//io/win32:file",
            "//util:utf",
        ],
        "//conditions:default": ["//io/posix:file"],
    }),
)

cc_library(
    name = "stream",
    srcs = ["stream.cc"],
    hdrs = ["stream.h"],
    deps = [
        ":file",
        ":file-utils",
    ],
)

cc_test(
    name = "stream_test",
    srcs = ["stream_test.cc"],
    deps = [
        ":memory-file",
        ":stream",
        "@googletest//:gtest_main",
    ],
)
