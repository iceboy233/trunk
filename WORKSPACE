workspace(name = "org_iceboy_trunk")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "com_github_google_flatbuffers",
    commit = "136d75fa6580ef87d1b7cbc243e617f21149852e",
    remote = "https://github.com/google/flatbuffers.git",
)

git_repository(
    name = "com_google_absl",
    commit = "aa844899c937bde5d2b24f276b59997e5b668bde",
    remote = "https://github.com/abseil/abseil-cpp.git",
)

git_repository(
    name = "com_google_googletest",
    commit = "703bd9caab50b139428cea1aaff9974ebee5742e",
    remote = "https://github.com/google/googletest.git",
)

git_repository(
    name = "org_boost_boost",
    commit = "d5b8d10a3ea6d43325da89d4ec3a796b274761c4",
    remote = "https://github.com/iceb0y/boost.git",
)

load("@org_boost_boost//:boost_deps.bzl", "boost_deps")
boost_deps()
