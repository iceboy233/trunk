def cc_embed_data(
        name,
        srcs,
        variable_name,
        namespace = None,
        visibility = None):
    generate_cc_embed_data(
        name = name + "_generated",
        srcs = srcs,
        variable_name = variable_name,
        namespace = namespace,
    )

    native.cc_library(
        name = name,
        srcs = [":{}_generated.cc".format(name)],
        hdrs = [":{}_generated.h".format(name)],
        visibility = visibility,
        deps = [
            "@abseil-cpp//absl/types:span",
            "@trunk//base:types",
            "@trunk//util:embed-data",
        ],
    )

def _generate_cc_embed_data(ctx):
    input_files = ctx.files.srcs
    output_cc = ctx.outputs.output_cc
    output_h = ctx.outputs.output_h

    ctx.actions.run(
        outputs = [output_cc, output_h],
        inputs = input_files,
        executable = ctx.executable.generator,
        arguments = [
            "--output_cc={}".format(output_cc.path),
            "--output_h={}".format(output_h.path),
            "--variable_name={}".format(ctx.attr.variable_name),
            "--cc_namespace={}".format(ctx.attr.namespace),
        ] + [f.path for f in input_files],
    )

    return [DefaultInfo(files = depset([output_cc, output_h]))]

generate_cc_embed_data = rule(
    implementation = _generate_cc_embed_data,
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
            mandatory = True,
        ),
        "variable_name": attr.string(mandatory = True),
        "namespace": attr.string(),
        "generator": attr.label(
            executable = True,
            cfg = "exec",
            default = Label(":generate-embed-data"),
        ),
    },
    outputs = {
        "output_cc": "%{name}.cc",
        "output_h": "%{name}.h",
    },
)
