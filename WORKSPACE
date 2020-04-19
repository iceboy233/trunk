workspace(name = "org_iceboy_trunk")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "boringssl",
    commit = "127d006498acc7c932b85d41d5f4ba3f0cbfbfd6",
    remote = "https://github.com/google/boringssl.git",
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
    commit = "703bd9caab50b139428cea1aaff9974ebee5742e",
    remote = "https://github.com/google/googletest.git",
)

git_repository(
    name = "org_boost_boost",
    commit = "759ce4a7e3bc7c5f0a0dddfee947b5758f9d7f70",
    remote = "https://github.com/iceb0y/boost.git",
)

load("@org_boost_boost//:boost_deps.bzl", "boost_deps")
boost_deps()
