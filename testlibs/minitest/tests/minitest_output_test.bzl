# Copyright 2024, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Defines rules and macros for testing the output of `minitest` executables.
"""

load("@bazel_skylib//rules:write_file.bzl", "write_file")
load("@rules_cc//cc:defs.bzl", "cc_binary")
load("//rules:binary_log.bzl", "binary_log")
load("//rules:diff_test.bzl", "diff_test")

visibility("private")

def replace(*args):
    """
    Helper to use with the `content_transformers` attr of `minitest_output_test`.

    Args:
      *args:
        Either a single argument that is a map containing multiple replacements of the form
          '{ "old1": "new1", "old2": "new2", ... }'
        or positional arguments passed to 'string.replace'
    Returns:
      A function that takes a string and returns that string, with the replacements made
    """
    if len(args) == 1:
        def dict_replace(string):
            for (old_value, new_value) in args[0].items():
                string = string.replace(old_value, new_value)
            return string

        return dict_replace
    else:
        return lambda string: string.replace(*args)

# Helper to use with the `content_transformers` attr of `minitest_output_test`.
identity = lambda x: x

def minitest_output_test(
        *,
        name = None,
        expected_content = None,
        content_transformers = {},
        expected_return_code = 0,
        srcs = None,
        deps = [
            "//testlibs/minitest",
        ],
        test_xml_output = False,
        **kwargs):
    """
    Macro to test the output of a `minitest` executable.

    This macro wraps the following:
    * creates a `cc_binary` target that depends on `//testlibs/minitest`
    * executes that `cc_binary` target to create a logfile
    * writes expected content to a file
    * defines a `diff_test` between both generated files

    Args:
      name: string
        Name for the `diff_test` targets. Intermediate targets use this as a
        basename.
      expected_content: string_list
        Console output contents, as lines of text. Newlines are added
        automatically after every line except the last one.
      content_transformers: dict (default `{}`)
        Dictionary where the arguments are configurations (keys for 'select')
        and values are unary functions with the following signature:
          f: string -> string

        This is used to transform content to handle output for different
        compilers (e.g. gcc vs clang).
      expected_return_code: int (default `0`)
        Expected return code of the minitest executable when executed.
      srcs: label_list
        Sources to compile in the `cc_binary` target.
      deps: label_list (default `[//testlibs/minitest]`)
        Dependencies for the `cc_binary` target.
      test_xml_output: bool (default `False`)
        Specify XML output when running `minitest` binary.
      **kwargs:
        Additional named arguments to pass to `cc_binary`.

    ```bzl
    minitest_output_test(
        name = "minitest_basic_111_output_test",
        srcs = ["minitest_basic_test.cpp"],
        local_defines = [
            "TEST_ONE_PASSES=true",
            "TEST_TWO_PASSES=true",
            "TEST_THREE_PASSES=true",
        ],
        expected_content = [
            "3 tests run. 3 PASSED. 0 FAILED",
            "",
        ],
    )

    minitest_output_test(
        name = "minitest_typed_111111_xml_output_test",
        srcs = ["minitest_typed_test.cpp"],
        expected_content = [
            '<?xml version="1.0" encoding="UTF-8"?>',
            '<testsuites name="AllTests">',
            '<testsuite name="TestSuiteForRefTypedTests">',
            '<testcase name="RefTest" file="testlibs/minitest/tests/minitest_typed_test.cpp" line="29" type_param="minitest_typed_tests::some_user_type&lt;int, &apos;a&apos;&gt; &amp;"/>',
            '<testcase name="RefTest" file="testlibs/minitest/tests/minitest_typed_test.cpp" line="29" type_param="double &amp;"/>',
            '<testcase name="RefTest" file="testlibs/minitest/tests/minitest_typed_test.cpp" line="29" type_param="char &amp;"/>',
            "</testsuite>",
            '<testsuite name="TestSuiteForTypedTests">',
            '<testcase name="TypedTest" file="testlibs/minitest/tests/minitest_typed_test.cpp" line="17" type_param="minitest_typed_tests::some_user_type&lt;int, &apos;a&apos;&gt;"/>',
            '<testcase name="TypedTest" file="testlibs/minitest/tests/minitest_typed_test.cpp" line="17" type_param="double"/>',
            '<testcase name="TypedTest" file="testlibs/minitest/tests/minitest_typed_test.cpp" line="17" type_param="char"/>',
            "</testsuite>",
            "</testsuites>",
            "",
        ],
        content_transformers = {
            "//configs/compiler:gcc": replace(" &amp", "&amp"),
        },
        local_defines = [
            "TYPED_TEST_PASSES=true",
            "SOME_USER_TYPE_VALUE=0",
            "REF_TEST_PASSES=true",
        ],
        test_xml_output = True,
    )
    ```
    """
    cc_binary(
        name = name + ".bin",
        srcs = srcs,
        deps = deps,
        tags = ["manual"],
        testonly = True,
        **kwargs
    )

    log_output_kwargs = {
        "file_descriptor": None,
        "env": {"XML_OUTPUT_FILE": "$@"},
    } if test_xml_output else {}

    binary_log(
        name = name + ".output",
        src = name + ".bin",
        out = name + ".actual.out",
        return_code = expected_return_code,
        testonly = True,
        **log_output_kwargs
    )

    transformed_content = {
        config: [transform(line) for line in expected_content]
        for config, transform in ({"//conditions:default": identity} | content_transformers).items()
    }

    write_file(
        name = name + ".expected",
        out = name + ".expected.out",
        content = select(transformed_content),
        tags = ["manual"],
    )

    diff_test(
        name = name,
        file1 = name + ".expected.out",
        file2 = name + ".actual.out",
    )
