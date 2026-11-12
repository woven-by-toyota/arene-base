# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""
Provides a rule for testing the captured output of a `cc_binary`.
"""

load("@rules_cc//cc:defs.bzl", _cc_binary = "cc_binary")
load("//rules:binary_log.bzl", _binary_log = "binary_log")
load("//rules:diff_test.bzl", _diff_test = "diff_test")

visibility("//...")

def cc_output_test(
        *,
        name,
        expected_output,
        expected_return_code = 0,
        tags = None,
        visibility = None,
        **kwargs):
    """Define a test that checks the output of a `cc_binary`.

    This macro defines three targets:
     * a `cc_binary` target with suffix `_binary` built from the given `srcs`
     * a `binary_log` target with suffix `_output` that runs the binary,
       captures its output, and asserts the binary exits with
       `expected_return_code`
     * a `diff_test` target with the given `name` that compares the captured
       output against `expected_output`

    The test passes if the binary exits with `expected_return_code` and its
    captured output matches `expected_output`.

    Args:
      name:
        Name of the `diff_test` target. The generated `cc_binary` and
        `binary_log` targets derive their names from this by appending
        `_binary` and `_output` respectively.
      expected_output:
        Label or file containing the expected binary output to diff against.
      expected_return_code:
        The return code the binary is expected to exit with. Defaults to `0`.
      tags:
        Tags applied to the `diff_test` target only. The generated
        `cc_binary` and `binary_log` targets are always tagged `manual` and
        do not receive these tags.
      visibility:
        Visibility for the `diff_test` target. The generated `cc_binary` and
        `binary_log` targets are always private. Defaults to the package's
        `default_visibility`.
      **kwargs:
        Additional kwargs passed to the `cc_binary` target (e.g. `srcs`,
        `deps`).
    """

    _cc_binary(
        name = name + "_binary",
        testonly = True,
        tags = ["manual"],
        visibility = ["//visibility:private"],
        **kwargs
    )

    _binary_log(
        name = name + "_output",
        testonly = True,
        src = ":{}_binary".format(name),
        env = {
            "GTEST_PRINT_TIME": "0",
        },
        return_code = expected_return_code,
        tags = ["manual"],
        visibility = ["//visibility:private"],
    )

    _diff_test(
        name = name,
        file1 = expected_output,
        file2 = ":{}_output".format(name),
        tags = tags,
        visibility = visibility,
    )
