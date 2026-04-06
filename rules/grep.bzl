# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Rules to run `grep`.
"""

visibility("//...")

_common_attrs = {
    "srcs": attr.label_list(
        allow_files = True,
        allow_empty = False,
        mandatory = True,
        providers = [DefaultInfo],
    ),
    "patterns": attr.string_list(
        allow_empty = False,
        mandatory = True,
    ),
    "pattern_syntax": attr.string(
        values = [
            "basic_regexp",
            "extended_regexp",
            "perl_regexp",
            "fixed_strings",
        ],
        default = "basic_regexp",
    ),
    "additional_options": attr.string_list(),
}

_return_codes = {
    "match": 0,
    "no_match": 1,
    "error": 2,
}

_grep_template = """
#!/usr/bin/env bash
set -euo pipefail

rc=0
grep {args} {output} || rc=$?
(({rc_compare}))
"""

def _grep_options(ctx, path_type):
    """
    Returns the list of options to `grep` for both rules.

    Args:
      ctx:
        Rule context value.
      path_type: string
        The type of path to use for files in `srcs`.
    """
    return ctx.attr.additional_options + [
        "--" + ctx.attr.pattern_syntax.replace("_", "-"),
    ] + [
        "--regexp=\"{}\"".format(pat)
        for pat in ctx.attr.patterns
    ] + [
        getattr(f, path_type)
        for src in ctx.attr.srcs
        for f in src[DefaultInfo].files.to_list()
    ]

def _impl(ctx):
    """
    Implementation function for `grep`
    """
    output = ctx.actions.declare_file(ctx.label.name + ".out")

    files = []
    for src in ctx.attr.srcs:
        files.extend(src[DefaultInfo].files.to_list())

    if ctx.attr.require_match:
        rc_compare = "rc == " + str(_return_codes["match"])
    else:
        rc_compare = "rc != " + str(_return_codes["error"])

    ctx.actions.run_shell(
        command = _grep_template.format(
            args = " ".join(_grep_options(ctx, "path")),
            output = "> " + output.path,
            rc_compare = rc_compare,
        ),
        inputs = files,
        outputs = [output],
        use_default_shell_env = True,
    )

    return [DefaultInfo(files = depset([output]))]

def _test_impl(ctx):
    """
    Implementation function for `grep_test`
    """
    executable = ctx.actions.declare_file(ctx.label.name + ".bash")

    ctx.actions.write(
        output = executable,
        content = _grep_template.format(
            rc_compare = "rc == {expected}".format(
                expected = _return_codes[ctx.attr.return_code],
            ),
            args = " ".join(_grep_options(ctx, "short_path")),
            output = "",
        ),
        is_executable = True,
    )

    return [
        DefaultInfo(
            executable = executable,
            runfiles = ctx.runfiles(
                transitive_files = depset(
                    transitive = [
                        src[DefaultInfo].files
                        for src in ctx.attr.srcs
                    ],
                ),
            ),
        ),
    ]

grep = rule(
    implementation = _impl,
    attrs = _common_attrs | {
        "require_match": attr.bool(default = True),
    },
    doc = """
    Search for patterns in files using `grep` and write the output to a file.
    """,
)

grep_test = rule(
    implementation = _test_impl,
    attrs = _common_attrs | {
        "return_code": attr.string(
            values = _return_codes.keys(),
            default = _return_codes.keys()[0],
        ),
    },
    test = True,
    doc = """
    Test for patterns in files using `grep`.
    """,
)
