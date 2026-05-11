# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Helper function to get the compile command used by Bazel.
"""

load("@rules_cc//cc:action_names.bzl", "ACTION_NAMES")
load("@rules_cc//cc:find_cc_toolchain.bzl", "find_cc_toolchain")

visibility("//...")

def get_compile_command(
        rule_ctx,
        compilation_ctx,
        action_name = ACTION_NAMES.cpp_compile):
    """
    Get the command used to compile a C++ target

    Args:
      rule_ctx: Rule context
      compilation_ctx: Compilation context
      action_name: string
        Type of compile action. Only handles `cpp_compile` for now.

    Returns:
      struct with compiler and options members.
    """
    if action_name != ACTION_NAMES.cpp_compile:
        fail("'{}' is not supported".format(action_name))

    fragment_flags = rule_ctx.fragments.cpp.cxxopts + rule_ctx.fragments.cpp.copts

    cc_toolchain = find_cc_toolchain(rule_ctx)

    feature_configuration = cc_common.configure_features(
        ctx = rule_ctx,
        cc_toolchain = cc_toolchain,
        requested_features = rule_ctx.features,
        unsupported_features = rule_ctx.disabled_features,
    )

    compile_variables = cc_common.create_compile_variables(
        cc_toolchain = cc_toolchain,
        feature_configuration = feature_configuration,
        user_compile_flags = fragment_flags,
        include_directories = compilation_ctx.includes,
        quote_include_directories = compilation_ctx.quote_includes,
        system_include_directories = depset(
            transitive = [
                compilation_ctx.system_includes,
                compilation_ctx.external_includes,
            ],
        ),
        framework_include_directories = compilation_ctx.framework_includes,
        preprocessor_defines = depset(
            transitive = [
                compilation_ctx.defines,
                compilation_ctx.local_defines,
            ],
        ),
    )

    compiler = cc_common.get_tool_for_action(
        feature_configuration = feature_configuration,
        action_name = action_name,
    )

    options = cc_common.get_memory_inefficient_command_line(
        feature_configuration = feature_configuration,
        action_name = action_name,
        variables = compile_variables,
    )

    return struct(
        compiler = compiler,
        options = options,
    )
