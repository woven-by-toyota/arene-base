# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Functions to conditionally create lists.
"""

load("@platforms//host:constraints.bzl", "HOST_CONSTRAINTS")

visibility("//...")

def _is(os):
    """
    Determines if the host platform contains `os`.
    """
    return any([
        elem.endswith("os:" + os)
        for elem in HOST_CONSTRAINTS
    ])

def _is_not(os):
    """
    Determines if the host platform does not contain `os`.
    """
    return not _is(os)

def _list_if(pred):
    """
    Returns a unary function that forwards a list if `pred` is true.
    """
    return lambda lst: lst if pred else []

# Utilities for conditionally creating lists depending on the host platform
# This is typically useful for attributes that are not configurable (e.g.
# `tags`).
if_host_platform = struct(
    is_linux = _list_if(_is("linux")),
    is_not_linux = _list_if(_is_not("linux")),
    is_macos = _list_if(_is("osx")),
    is_not_macos = _list_if(_is_not("osx")),
)
