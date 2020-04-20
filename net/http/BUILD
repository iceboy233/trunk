package(default_visibility = ["//visibility:public"])

cc_library(
    name = "beast",
    hdrs = ["beast.h"],
    deps = ["@org_boost_boost//:beast"],
)

cc_library(
    name = "client",
    srcs = ["client.cc"],
    hdrs = ["client.h"],
    deps = [
        ":beast",
        "//net:asio",
        "//net:timer-list",
        "@boringssl//:ssl",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@org_boost_boost//:asio",
        "@org_boost_boost//:smart_ptr",
    ],
)
