# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""re-export rules from @rules_build_error.
"""

load(
    "@rules_build_error//lang/cc:defs.bzl",
    _cc_build_error_test = "cc_build_error_test",
    _inline_src = "inline_src",
)
load(
    "@rules_build_error//matcher:defs.bzl",
    _matcher = "matcher",
)

visibility("//...")

_NO_COLOR = {
    "//configs/compiler:clang": ["-fno-color-diagnostics"],
    "//configs/compiler:arm_clang": ["-fno-color-diagnostics"],
    "//configs/compiler:gcc_or_qcc": ["-fdiagnostics-color=never"],
    "//conditions:default": [],
}

def cc_build_error_test(*, name, **kwargs):
    """Test rule checking `cc_build_error` builds.

    Args:
        name(str): Name of the test target.
        **kwargs(dict): Receives the same keyword arguments as `cc_build_error`.

    Wrapper around
    @arene-rules-build-error//lang/cc:defs.bzl%cc_build_error_test that always
    prepends `copts`` to disable color. This can be overriden by explicitly
    setting color options in `copts`.
    """
    copts = kwargs.pop("copts", [])
    deps = kwargs.pop("deps", [])

    _cc_build_error_test(
        name = name,
        copts = select(_NO_COLOR) + copts,
        deps = deps + select({
            "//configs:toolchain_stdlib_enabled": [],
            "//configs:toolchain_stdlib_disabled": ["//stdlib"],
        }),
        **kwargs
    )

inline_src = _inline_src
matcher = _matcher
