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
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "line-reader",
    srcs = ["line-reader.cc"],
    hdrs = ["line-reader.h"],
    deps = [
        ":file",
        "@com_google_absl//absl/types:span",
    ],
)

cc_test(
    name = "line-reader_test",
    srcs = ["line-reader_test.cc"],
    deps = [
        ":line-reader",
        ":memory-file",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "memory-file",
    srcs = ["memory-file.cc"],
    hdrs = ["memory-file.h"],
    deps = [":file"],
)
