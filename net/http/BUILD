package(default_visibility = ["//visibility:public"])

cc_library(
    name = "beast",
    hdrs = ["beast.h"],
    deps = ["@boost.beast"],
)

cc_library(
    name = "client",
    srcs = ["client.cc"],
    hdrs = ["client.h"],
    deps = [
        ":beast",
        "//net:asio",
        "//net:timer-list",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/strings",
        "@boost.asio",
        "@boost.smart_ptr",
        "@boringssl//:ssl",
    ],
)

cc_library(
    name = "server",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [
        ":beast",
        "//net:asio",
        "//net:timer-list",
        "@boost.smart_ptr",
    ],
)
