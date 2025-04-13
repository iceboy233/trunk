package(default_visibility = ["//visibility:public"])

cc_library(
    name = "client",
    srcs = ["client.cc"],
    hdrs = ["client.h"],
    deps = [
        ":operation-key",
        ":wire-structs",
        "//net:asio",
        "//security:key",
        "//security/keys:zero_key_cc",
        "//util:fibonacci-sequence",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/random",
        "@abseil-cpp//absl/types:span",
        "@boost.icl",
        "@boost.smart_ptr",
    ],
)

cc_library(
    name = "flatbuffers-handler",
    hdrs = ["flatbuffers-handler.h"],
    deps = [
        ":handler",
        "//security:key",
        "//util:flatbuffers",
    ],
)

cc_library(
    name = "flatbuffers-request",
    hdrs = ["flatbuffers-request.h"],
    deps = [
        ":client",
        "//net:asio",
        "//util:flatbuffers",
    ],
)

cc_library(
    name = "handler",
    hdrs = ["handler.h"],
    deps = ["//security:key"],
)

cc_library(
    name = "operation-key",
    srcs = ["operation-key.cc"],
    hdrs = ["operation-key.h"],
)

cc_test(
    name = "rpc_test",
    srcs = ["rpc_test.cc"],
    deps = [
        ":client",
        ":flatbuffers-request",
        ":server",
        "//net:asio",
        "//net:blocking-result",
        "//net/rpc/flatbuffers:list",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "server",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [
        ":flatbuffers-handler",
        ":operation-key",
        ":wire-structs",
        "//base:logging",
        "//net:asio",
        "//net/rpc/flatbuffers:list",
        "//security:key",
        "//security:keystore",
        "//security/keys:zero_key_cc",
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/types:span",
        "@boost.icl",
        "@boost.smart_ptr",
    ],
)

cc_library(
    name = "wire-structs",
    hdrs = ["wire-structs.h"],
    deps = ["//base:packed"],
)
