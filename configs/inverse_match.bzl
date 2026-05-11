# Copyright 2024, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""Provide an inverse_match rule to create a config setting for use with select that is the inverse of another setting."""

def inverse_match(name, source, visibility = ["//visibility:private"]):
    """Create a config setting that matches if and only if the source constraint does not match"""

    native.alias(
        name = name,
        actual = select({
            source: "//configs:never_matches",
            "//conditions:default": "//configs:always_matches",
        }),
        visibility = visibility,
    )
