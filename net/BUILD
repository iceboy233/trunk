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
    deps = [":asio"],
)

cc_test(
    name = "blocking-result_test",
    srcs = ["blocking-result_test.cc"],
    deps = [
        ":blocking-result",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "ip-encoding",
    srcs = ["ip-encoding.cc"],
    hdrs = ["ip-encoding.h"],
    deps = [
        ":asio",
        "@com_google_absl//absl/types:span",
    ],
)

cc_test(
    name = "ip-encoding_test",
    srcs = ["ip-encoding_test.cc"],
    deps = [
        ":asio",
        ":ip-encoding",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "timer-list",
    hdrs = ["timer-list.h"],
    deps = [":asio"],
)

cc_test(
    name = "timer-list_test",
    srcs = ["timer-list_test.cc"],
    deps = [
        ":asio",
        ":timer-list",
        "//net/testing:timer",
        "//util/testing:clock",
        "@com_google_googletest//:gtest_main",
    ],
)
