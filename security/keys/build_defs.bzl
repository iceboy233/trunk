def key_cc_library(name, srcs, visibility=None):
    native.cc_library(
        name = name,
        srcs = [":{}_srcs".format(name)],
        hdrs = [":{}_srcs".format(name)],
        deps = ["//security:key"],
        visibility = visibility,
    )

    outs = []
    for src in srcs:
        outs.append("{}.h".format(src))
        outs.append("{}.cc".format(src))

    # TODO(iceboy): Support multiple keys in a single library definition.
    native.genrule(
        name = "{}_srcs".format(name),
        srcs = srcs,
        outs = outs,
        cmd = "$(location //security/keys:generate-key-cc) \"$<\" \"$(@D)\"",
        tools = ["//security/keys:generate-key-cc"],
        visibility = ["//visibility:private"],
    )
