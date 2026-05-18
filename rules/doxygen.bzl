# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""Bazel rule for generating Doxygen documentation.

Runs Doxygen with a provided configuration file and produces an output
directory containing the generated documentation.

```
load("//tools/doxygen:doxygen.bzl", "doxygen")

doxygen(
    name = "test_specification",
    config = ":test_specification_config",
    srcs = [
        "doxygen-awesome.css",
        "test_report_intro.md",
        "test_report_layout",
    ],
)
```
"""

visibility("//...")

def _doxygen_impl(ctx):
    output_dir = ctx.actions.declare_directory(ctx.attr.name)

    inputs = [ctx.file.config] + ctx.files.srcs

    ctx.actions.run_shell(
        inputs = inputs,
        outputs = [output_dir],
        tools = [ctx.executable._doxygen],
        command = """
set -euo pipefail

(cat {config}; echo "OUTPUT_DIRECTORY = {output}") | {doxygen} -
        """.format(
            output = output_dir.path,
            doxygen = ctx.executable._doxygen.path,
            config = ctx.file.config.path,
        ),
        use_default_shell_env = True,
        mnemonic = "Doxygen",
        progress_message = "Generating Doxygen docs: {}".format(ctx.label),
    )

    return [DefaultInfo(files = depset([output_dir]))]

doxygen = rule(
    implementation = _doxygen_impl,
    attrs = {
        "config": attr.label(
            mandatory = True,
            allow_single_file = True,
            doc = "Doxygen configuration file.",
        ),
        "srcs": attr.label_list(
            allow_files = True,
            doc = "Additional input files (css, layout, images, etc.).",
        ),
        "_doxygen": attr.label(
            default = "@doxygen",
            executable = True,
            cfg = "exec",
        ),
    },
)
