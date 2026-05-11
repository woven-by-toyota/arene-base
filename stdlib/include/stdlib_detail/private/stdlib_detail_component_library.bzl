# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Define a macro to simplify definition of `cc_library` targets in
`stdlib_detail`.
"""

load("@rules_cc//cc:defs.bzl", "cc_library")
load(
    "//rules:transitions.bzl",
    "transition_use_toolchain_stdlib",
)

visibility("//stdlib/include/stdlib_detail/...")

def stdlib_detail_component_library(*, name = None, deps = [], **kwargs):
    """
    Define a `cc_library` and associated targets for a component in `stdlib_detail`.

    Args:
      name: string
        Name for this target.
      deps: label_list
        Component dependencies.
      **kwargs:
        Additional kwargs passed to the `cc_library` target.
    """
    native.filegroup(
        name = name + "_hdrs",
        srcs = [name + ".hpp"],
    )

    cc_library(
        name = name + "_deps",
        tags = ["manual"],
        deps = deps,
    )

    transition_use_toolchain_stdlib(
        name = name + "_deps_without_toolchain_stdlib",
        src = ":" + name + "_deps",
        tags = ["manual"],
        value = False,
    )

    cc_library(
        name = name,
        hdrs = [":" + name + "_hdrs"],
        deps = [":" + name + "_deps_without_toolchain_stdlib"],
        **kwargs
    )
