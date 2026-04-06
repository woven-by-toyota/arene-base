# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Generic rule to check that a command will succeed.
"""

def _impl(ctx):
    output = ctx.actions.declare_file(ctx.label.name + ".touch")

    ctx.actions.run_shell(
        command = "({cmd} && touch {out}) || (echo '{msg}' && false)".format(
            cmd = ctx.expand_location(
                ctx.attr.command,
                targets = ctx.attr.data,
            ),
            out = output.path,
            msg = ctx.attr.error_message,
        ),
        outputs = [output],
        inputs = ctx.files.data,
        use_default_shell_env = True,
    )

    return [
        DefaultInfo(
            files = depset([output]),
            runfiles = ctx.runfiles(
                files = ctx.files.data,
            ).merge_all([
                target[DefaultInfo].default_runfiles
                for target in ctx.attr.data
            ]),
        ),
    ]

command_succeeds = rule(
    implementation = _impl,
    attrs = {
        "command": attr.string(),
        "error_message": attr.string(),
        "data": attr.label_list(
            allow_files = True,
            cfg = "exec",
            providers = [DefaultInfo],
        ),
    },
    doc = """
    Creates a dummy file after a successful command. This dummy file can be used
    as a dependency.

    Unlike 'genrule', this rule has access to environment variables specified
    with --action_env.
    """,
)
