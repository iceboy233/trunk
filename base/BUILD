package(default_visibility = ["//visibility:public"])

cc_library(
    name = "flags",
    srcs = ["flags.cc"],
    hdrs = ["flags.h"],
    deps = [
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
    ],
)

cc_library(
    name = "logging",
    srcs = ["logging.cc"],
    hdrs = ["logging.h"],
    deps = ["@org_boost_boost//:log"],
)

cc_library(
    name = "packed",
    hdrs = ["packed.h"],
)
