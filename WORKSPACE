workspace(name = "org_iceboy_trunk")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "boringssl",
    commit = "127d006498acc7c932b85d41d5f4ba3f0cbfbfd6",
    remote = "https://github.com/google/boringssl.git",
)

git_repository(
    name = "com_github_google_benchmark",
    commit = "c5b2fe9357b3862b7f99b94d7999002dcf269faf",
    remote = "https://github.com/google/benchmark.git",
)

git_repository(
    name = "com_github_google_flatbuffers",
    commit = "6df40a2471737b27271bdd9b900ab5f3aec746c7",
    remote = "https://github.com/google/flatbuffers.git",
)

git_repository(
    name = "com_google_absl",
    commit = "62f05b1f57ad660e9c09e02ce7d591dcc4d0ca08",
    remote = "https://github.com/abseil/abseil-cpp.git",
)

git_repository(
    name = "com_google_googletest",
    commit = "dcc92d0ab6c4ce022162a23566d44f673251eee4",
    remote = "https://github.com/google/googletest.git",
)

git_repository(
    name = "org_boost_boost",
    commit = "13413ef10592ca33bec6bcadf67f62ced07a1b7f",
    remote = "https://github.com/iceboy233/boost.git",
)

load("@org_boost_boost//:boost_deps.bzl", "boost_deps")
boost_deps()
