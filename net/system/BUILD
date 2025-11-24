package(default_visibility = ["//visibility:public"])

cc_library(
    name = "connector",
    srcs = ["connector.cc"],
    hdrs = ["connector.h"],
    deps = [
        ":tcp-socket-stream",
        ":udp-socket-datagram",
        "//net:asio",
        "//net/types:interface",
    ],
)

cc_library(
    name = "tcp-socket-stream",
    srcs = ["tcp-socket-stream.cc"],
    hdrs = ["tcp-socket-stream.h"],
    deps = [
        "//net:asio",
        "//net/types:interface",
    ],
)

cc_library(
    name = "udp-socket-datagram",
    srcs = ["udp-socket-datagram.cc"],
    hdrs = ["udp-socket-datagram.h"],
    deps = [
        "//net:asio",
        "//net/types:interface",
    ],
)
