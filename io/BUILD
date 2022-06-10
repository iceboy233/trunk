package(default_visibility = ["//visibility:public"])

cc_library(
    name = "file",
    hdrs = ["file.h"],
    deps = ["//base:types"],
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
