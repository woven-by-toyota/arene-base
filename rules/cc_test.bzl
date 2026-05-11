# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Provides rules that provides C++ test target, including a target that build
using //stdlib.
"""

load(
    "@rules_cc//cc:defs.bzl",
    _cc_library = "cc_library",
    _cc_test = "cc_test",
)
load(
    "//rules:transitions.bzl",
    "transition_use_toolchain_stdlib",
)

visibility("//...")

def cc_test(**kwargs):
    """
    Define a replacement 'cc_test' target.

    This target is disabled if the toolchain provided C++ standard library is
    not available.

    Args:
      **kwargs:
        Additional kwargs passed to the `cc_test` target

    Returns:
        list of the test targets defined by this macro
    """
    deps = kwargs.pop("deps", []) + [
        "//configs:incompatible_with_toolchain_stdlib_disabled",
    ]

    name = kwargs.pop("name")

    _cc_test(
        name = name,
        deps = deps,
        **kwargs
    )

    return [name]

def cc_test_with_variants(**kwargs):
    """
    Define a standard 'cc_test' target and additional targets for testing

    This macro defines two targets:
     * a standard 'cc_test' target
     * an identically named target with suffix `.without_toolchain_stdlib`

    The `.without_toolchain_stdlib` effectively applies a transition to the
    base `cc_test` target - configuring it to avoid using the toolchain
    provided stdlib and instead use `//stdlib`.

    Args:
      **kwargs:
        Additional kwargs passed to the `cc_test` target

    Returns:
        list of the test targets defined by this macro
    """
    deps = kwargs.pop("deps", []) + select({
        "//configs:toolchain_stdlib_enabled": [],
        "//configs:toolchain_stdlib_disabled": ["//stdlib"],
    })

    _cc_test(
        deps = deps + [
            # avoid building the test target twice
            # if this flag is globally set to disabled
            "//configs:incompatible_with_toolchain_stdlib_disabled",
        ],
        **kwargs
    )

    name = kwargs.pop("name")

    _cc_library(
        name = name + ".deps",
        testonly = True,
        deps = deps,
        visibility = ["//visibility:private"],
        tags = ["manual"],
    )

    transition_use_toolchain_stdlib(
        name = name + ".deps.without_toolchain_stdlib",
        src = ":" + name + ".deps",
        value = False,
        visibility = ["//visibility:private"],
        tags = ["manual"],
        testonly = True,
    )

    _cc_test(
        name = name + ".without_toolchain_stdlib",
        deps = [name + ".deps.without_toolchain_stdlib"],
        **kwargs
    )

    return [
        name,
        name + ".without_toolchain_stdlib",
    ]
