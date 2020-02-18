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

cc_library(
    name = "blocking-result",
    hdrs = ["blocking-result.h"],
    deps = [
        ":asio",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "blocking-result_test",
    srcs = ["blocking-result_test.cc"],
    deps = [
        ":blocking-result",
        "@com_google_googletest//:gtest_main",
    ],
)
