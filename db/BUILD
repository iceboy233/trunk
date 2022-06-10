package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sstable",
    srcs = ["sstable.cc"],
    hdrs = ["sstable.h"],
    deps = [
        "//base:types",
        "//io:file",
        "@com_google_absl//absl/algorithm:container",
        "@org_boost_boost//:endian",
    ],
)

cc_test(
    name = "sstable_test",
    srcs = ["sstable_test.cc"],
    deps = [
        ":sstable",
        "//io/posix:file",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_googletest//:gtest_main",
    ],
)
