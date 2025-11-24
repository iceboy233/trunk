package(default_visibility = ["//visibility:public"])

cc_library(
    name = "interface",
    hdrs = [
        "connector.h",
        "datagram.h",
        "resolver.h",
        "stream.h",
    ],
    deps = ["//net:asio"],
)

cc_library(
    name = "addr-port",
    srcs = ["addr-port.cc"],
    hdrs = ["addr-port.h"],
    deps = [
        "//net:asio",
        "//net:asio-hash",
        "//util:strings",
        "@abseil-cpp//absl/strings",
    ],
)

cc_test(
    name = "addr-port_test",
    srcs = ["addr-port_test.cc"],
    deps = [
        ":addr-port",
        "@abseil-cpp//absl/hash:hash_testing",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "host",
    srcs = ["host.cc"],
    hdrs = ["host.h"],
    deps = [
        "//net:asio",
        "//net:asio-hash",
    ],
)

cc_test(
    name = "host_test",
    srcs = ["host_test.cc"],
    deps = [
        ":host",
        "@abseil-cpp//absl/hash:hash_testing",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "host-port",
    srcs = ["host-port.cc"],
    hdrs = ["host-port.h"],
    deps = [
        ":addr-port",
        ":host",
        "//net:asio",
        "//util:strings",
        "@abseil-cpp//absl/strings",
    ],
)

cc_test(
    name = "host-port_test",
    srcs = ["host-port_test.cc"],
    deps = [
        ":host-port",
        "@abseil-cpp//absl/hash:hash_testing",
        "@googletest//:gtest_main",
    ],
)
