load(":cc_toolchain_config.bzl", "cc_toolchain_config")

filegroup(name = "empty")

cc_toolchain_suite(
    name = "linux_crosstool",
    toolchains = {
        "aarch64": ":linux_aarch64_toolchain",
        "k8": ":linux_x86_64_toolchain",
    },
)

cc_toolchain(
    name = "linux_aarch64_toolchain",
    toolchain_identifier = "linux-aarch64-toolchain",
    toolchain_config = ":linux_aarch64_toolchain_config",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
)

cc_toolchain_config(
    name = "linux_aarch64_toolchain_config",
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

cc_toolchain(
    name = "linux_x86_64_toolchain",
    toolchain_identifier = "linux-x86_64-toolchain",
    toolchain_config = ":linux_x86_64_toolchain_config",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
)

cc_toolchain_config(
    name = "linux_x86_64_toolchain_config",
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
