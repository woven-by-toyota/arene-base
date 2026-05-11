# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Provides a rule that provides multiple cc_library targets, including private
cc_library targets to simplify testing //stdlib.
"""

load("@rules_cc//cc:defs.bzl", "cc_library")
load(
    "//rules:transitions.bzl",
    "transition_use_toolchain_stdlib",
)

visibility("//...")

def cc_library_with_variants(**kwargs):
    """
    Define a standard 'cc_library' target and additional targets for testing

    This macro defines two targets:
     * a standard 'cc_library' target
     * an identically named target with suffix `.without_toolchain_stdlib`

    The `.without_toolchain_stdlib` applies a transition to the base
    `cc_library` target - configuring it and to avoid using the toolchain
    provided stdlib and instead use `//stdlib`.

    Args:
      **kwargs:
        Additional kwargs passed to the `cc_library` target

    Returns:
        list of the targets defined by this macro
    """
    name = kwargs.pop("name")

    cc_library(
        name = name,
        **kwargs
    )

    # this target is used in testing //stdlib and needs to avoid using the
    # toolchain provided C++ standard libraray
    transition_use_toolchain_stdlib(
        name = name + ".without_toolchain_stdlib",
        src = ":" + name,
        value = False,
        visibility = ["//:__subpackages__"],
        tags = ["manual"],
        testonly = kwargs.get("testonly"),
    )

    return [
        name,
        name + ".without_toolchain_stdlib",
    ]
