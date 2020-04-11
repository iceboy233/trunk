package(default_visibility = ["//visibility:public"])

load("//security/keys:build_defs.bzl", "key_cc_library")

key_cc_library(
    name = "zero_key_cc",
    srcs = ["zero.key"],
)

py_binary(
    name = "generate-key-cc",
    srcs = ["generate-key-cc.py"],
)
