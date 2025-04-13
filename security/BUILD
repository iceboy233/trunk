package(default_visibility = ["//visibility:public"])

cc_library(
    name = "key",
    srcs = ["key.cc"],
    hdrs = ["key.h"],
    deps = [
        ":types",
        "@abseil-cpp//absl/types:span",
        "@boringssl//:crypto",
    ],
)

cc_test(
    name = "key_test",
    srcs = ["key_test.cc"],
    deps = [
        ":key",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "keystore",
    srcs = ["keystore.cc"],
    hdrs = ["keystore.h"],
    deps = [
        ":key",
        "@abseil-cpp//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "keystore_test",
    srcs = ["keystore_test.cc"],
    deps = [
        ":keystore",
        "@googletest//:gtest_main",
    ],
)

cc_library(
    name = "types",
    hdrs = ["types.h"],
)
