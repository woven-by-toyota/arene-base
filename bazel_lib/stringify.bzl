# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Functions to stringify items for substitution.
"""

load("@bazel_skylib//lib:shell.bzl", "shell")

visibility("//...")

def _as_string(item):
    return shell.quote(item)

def _as_list(items):
    return ("[" + ", ".join([
        shell.quote(item)
        for item in items
    ]) + "]")

def _as_dict(items):
    return ("{" + ", ".join([
        shell.quote(key) + ": " + shell.quote(value)
        for key, value in items.items()
    ]) + "}")

def _stringify(item):
    if type(item) == "string":
        return _as_string(item)
    if type(item) == "list":
        return _as_list(item)
    if type(item) == "dict":
        return _as_dict(item)
    else:
        fail("unhandled type")

# utilitiies for stringifying starlark items, typically used for subsitution in
# a template
stringify = struct(
    as_string = _as_string,
    as_list = _as_list,
    as_dict = _as_dict,
    stringify = _stringify,
)
