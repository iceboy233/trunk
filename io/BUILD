package(default_visibility = ["//visibility:public"])

cc_library(
    name = "file",
    hdrs = ["file.h"],
    deps = ["@com_google_absl//absl/types:span"],
)

cc_library(
    name = "posix-file",
    srcs = ["posix-file.cc"],
    hdrs = ["posix-file.h"],
    deps = [":file"],
)

cc_test(
    name = "posix-file_test",
    srcs = ["posix-file_test.cc"],
    deps = [
        ":posix-file",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ]
)
