package(default_visibility = ["//visibility:public"])

cc_library(
    name = "flags",
    srcs = ["flags.cc"],
    hdrs = ["flags.h"],
    deps = [
        "@abseil-cpp//absl/base:no_destructor",
        "@abseil-cpp//absl/container:inlined_vector",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/flags:usage",
    ],
)

cc_library(
    name = "logging",
    srcs = ["logging.cc"],
    hdrs = ["logging.h"],
    deps = ["@boost.log"],
)

cc_library(
    name = "packed",
    hdrs = ["packed.h"],
)

cc_library(
    name = "types",
    hdrs = ["types.h"],
    deps = ["@abseil-cpp//absl/types:span"],
)
