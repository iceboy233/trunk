package(default_visibility = ["//visibility:public"])

cc_library(
    name = "asio",
    hdrs = ["asio.h"],
    deps = ["@org_boost_boost//:asio"],
)

cc_test(
    name = "asio_test",
    srcs = ["asio_test.cc"],
    deps = [
        ":asio",
        "@com_google_googletest//:gtest_main",
    ],
)
