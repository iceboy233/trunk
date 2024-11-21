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
        "@com_google_absl//absl/functional:any_invocable",
        "@com_google_absl//absl/types:span",
    ],
)
