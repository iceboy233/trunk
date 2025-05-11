package(default_visibility = ["//visibility:public"])

cc_library(
    name = "interface",
    hdrs = [
        "connector.h",
        "datagram.h",
        "handler.h",
        "resolver.h",
        "stream.h",
    ],
    deps = [
        "//net:asio",
        "//net/types:host-port",
        "@abseil-cpp//absl/functional:any_invocable",
        "@abseil-cpp//absl/types:span",
    ],
)
