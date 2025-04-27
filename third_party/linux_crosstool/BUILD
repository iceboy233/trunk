load(":cc_toolchain_config.bzl", "cc_toolchain_config")

package(default_visibility = ["//visibility:public"])

filegroup(name = "empty")

toolchain(
    name = "linux-aarch64_toolchain",
    target_compatible_with = [
        "@platforms//cpu:aarch64",
        "@platforms//os:linux",
    ],
    toolchain = ":linux-aarch64_cc_toolchain",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

cc_toolchain(
    name = "linux-aarch64_cc_toolchain",
    toolchain_identifier = "linux-aarch64",
    toolchain_config = ":linux-aarch64_cc_toolchain_config",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
)

cc_toolchain_config(
    name = "linux-aarch64_cc_toolchain_config",
    target = "aarch64-linux-gnu",
    target_cpu = "aarch64",
    dbg_compile_flags = [
        "-g",
        "-fno-omit-frame-pointer",
    ],
    opt_compile_flags = [
        "-g0",
        "-O2",
        "-DNDEBUG",
        "-ffunction-sections",
        "-fdata-sections",
    ],
    link_flags = [
        "-static-libgcc",
        "-l:libstdc++.a",
        "-lm",
    ],
    opt_link_flags = [
        "-Wl,--gc-sections",
    ],
)

toolchain(
    name = "linux-x86_32_toolchain",
    target_compatible_with = [
        "@platforms//cpu:x86_32",
        "@platforms//os:linux",
    ],
    toolchain = ":linux-x86_32_cc_toolchain",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

cc_toolchain(
    name = "linux-x86_32_cc_toolchain",
    toolchain_identifier = "linux-x86_32",
    toolchain_config = ":linux-x86_32_cc_toolchain_config",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
)

cc_toolchain_config(
    name = "linux-x86_32_cc_toolchain_config",
    target = "i686-linux-gnu",
    target_cpu = "i686",
    dbg_compile_flags = [
        "-g",
        "-fno-omit-frame-pointer",
    ],
    opt_compile_flags = [
        "-g0",
        "-O2",
        "-DNDEBUG",
        "-ffunction-sections",
        "-fdata-sections",
    ],
    link_flags = [
        "-static-libgcc",
        "-l:libstdc++.a",
        "-lm",
    ],
    opt_link_flags = [
        "-Wl,--gc-sections",
    ],
)

toolchain(
    name = "linux-x86_64_toolchain",
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],
    toolchain = ":linux-x86_64_cc_toolchain",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

cc_toolchain(
    name = "linux-x86_64_cc_toolchain",
    toolchain_identifier = "linux-x86_64",
    toolchain_config = ":linux-x86_64_cc_toolchain_config",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
)

cc_toolchain_config(
    name = "linux-x86_64_cc_toolchain_config",
    target = "x86_64-linux-gnu",
    target_cpu = "x86_64",
    dbg_compile_flags = [
        "-g",
        "-fno-omit-frame-pointer",
    ],
    opt_compile_flags = [
        "-g0",
        "-O2",
        "-DNDEBUG",
        "-ffunction-sections",
        "-fdata-sections",
    ],
    link_flags = [
        "-static-libgcc",
        "-l:libstdc++.a",
        "-lm",
    ],
    opt_link_flags = [
        "-Wl,--gc-sections",
    ],
)
