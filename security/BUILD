package(default_visibility = ["//visibility:public"])

cc_library(
    name = "key",
    srcs = ["key.cc"],
    hdrs = ["key.h"],
    deps = [
        ":types",
        "@boringssl//:crypto",
        "@com_google_absl//absl/types:span",
    ],
)

cc_test(
    name = "key_test",
    srcs = ["key_test.cc"],
    deps = [
        ":key",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "keystore",
    srcs = ["keystore.cc"],
    hdrs = ["keystore.h"],
    deps = [
        ":key",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "keystore_test",
    srcs = ["keystore_test.cc"],
    deps = [
        ":keystore",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "types",
    hdrs = ["types.h"],
)
