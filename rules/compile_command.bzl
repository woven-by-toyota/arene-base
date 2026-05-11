# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Provides a rule to obtain the compile command of a target.
"""

load("@rules_cc//cc:find_cc_toolchain.bzl", "CC_TOOLCHAIN_TYPE")
load("//rules/impl:get_compile_command.bzl", "get_compile_command")

visibility("//...")

def _command(rule_ctx, compilation_ctx):
    info = get_compile_command(rule_ctx, compilation_ctx)
    return [info.compiler] + info.options

def _impl(ctx):
    """
    Implementation function for `compile_command`
    """
    commands = []

    for src in ctx.attr.srcs:
        compilation_context = src[CcInfo].compilation_context
        commands.append(" ".join(_command(ctx, compilation_context)))

    output = ctx.actions.declare_file(ctx.attr.name + ".out")

    ctx.actions.write(
        output,
        content = "\n".join(commands),
    )

    return [DefaultInfo(files = depset([output]))]

compile_command = rule(
    implementation = _impl,
    attrs = {
        "srcs": attr.label_list(
            allow_empty = False,
            providers = [CcInfo],
            doc = "Files for which to obtain the compile commands",
        ),
    },
    fragments = ["cpp"],
    toolchains = [CC_TOOLCHAIN_TYPE],
    doc = """
    Rules used to obtain the compilation command for the specified C++ targets.

    It does not provide compilation commands for all transitive dependencies.
    """,
)
