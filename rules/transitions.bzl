# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Provides rules to transition a non-binary type target by setting a flag value.
"""

def _transition_target_impl(ctx):
    src = ctx.attr.src[0]

    return [
        src[provider]
        for provider in [
            # we can't forward DefaultInfo if the target is executable
            DefaultInfo,
            CcInfo,
        ]
        if provider in src
    ]

_set_platform = transition(
    implementation = lambda _settings, attr: [
        {"//command_line_option:platforms": str(attr.platform)},
    ],
    inputs = [],
    outputs = ["//command_line_option:platforms"],
)

transition_platform = rule(
    implementation = _transition_target_impl,
    attrs = {
        "src": attr.label(
            mandatory = True,
            cfg = _set_platform,
            providers = [DefaultInfo],
        ),
        "platform": attr.label(
            mandatory = True,
        ),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
    },
    doc = """
    Rule to transition a target to a different platform.

    Used to build a cc target under a specific platform, passing through
    DefaultInfo and CcInfo providers (if they exist).
    """,
)

def transition_generated_platform(*, name = None, src = None, platform_constraint_values = None, **kwargs):
    """
    Macro that transitions a cc target to a different platform.

    Creates a platform from the given constraint values and builds `src`
    under that platform, passing through both DefaultInfo and CcInfo.

    Args:
      name: string
        Name for the target.
      src: label
        Target to build under the platform transition.
      platform_constraint_values: label_list
        Constraint values of the desired platform.
      **kwargs:
        Additional named arguments to pass to the underlying rule.
    """

    native.platform(
        name = name + ".platform.generated",
        constraint_values = platform_constraint_values,
    )

    transition_platform(
        name = name,
        src = src,
        platform = name + ".platform.generated",
        **kwargs
    )

def _transition_rule_for(*, bool_flag = None, string_flag = None):
    if bool(bool_flag) == bool(string_flag):
        fail("only a single argument can be set")

    if bool_flag:
        flag = bool_flag
        value_attr = attr.bool
    else:
        flag = string_flag
        value_attr = attr.string

    _flag_transition = transition(
        implementation = lambda _settings, attr: [
            {flag: attr.value},
        ],
        inputs = [],
        outputs = [flag],
    )

    return rule(
        implementation = _transition_target_impl,
        attrs = {
            "value": value_attr(
                mandatory = True,
            ),
            "src": attr.label(
                mandatory = True,
                cfg = _flag_transition,
                providers = [DefaultInfo],
            ),
            "_allowlist_function_transition": attr.label(
                default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
            ),
        },
        doc = """
        Rule to provide a flag configuration transition.

        Used to set a value for flag `{flag}`.

        Args:
          name: string
            Name for the target.
          value: bool|string
            Value for `{flag}` flag.
          src: label
            Underlying cc_library target.
        """.format(
            flag = bool_flag,
        ),
    )

transition_use_toolchain_stdlib = _transition_rule_for(
    bool_flag = "//configs:use_toolchain_stdlib",
)
transition_capture_contract_stacktraces = _transition_rule_for(
    bool_flag = "//configs:capture_contract_stacktraces",
)
transition_use_exceptions = _transition_rule_for(
    bool_flag = "//configs:use_exceptions",
)
transition_export_type = _transition_rule_for(
    string_flag = "//configs:export_type",
)
