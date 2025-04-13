package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sstable",
    srcs = ["sstable.cc"],
    hdrs = ["sstable.h"],
    deps = [
        "//base:types",
        "//io:file",
        "@abseil-cpp//absl/algorithm:container",
        "@boost.endian",
    ],
)

cc_test(
    name = "sstable_test",
    srcs = ["sstable_test.cc"],
    deps = [
        ":sstable",
        "//io:memory-file",
        "@abseil-cpp//absl/strings:str_format",
        "@googletest//:gtest_main",
    ],
)
