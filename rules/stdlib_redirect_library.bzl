# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Provides a macro that allows redirection of standard library headers.
"""

load("@bazel_skylib//rules:expand_template.bzl", "expand_template")
load("@rules_cc//cc:defs.bzl", "cc_library")

visibility("//...")

def _replace_invalid_characters(name):
    """
    Replace invalid characters in macro names with "_"
    """
    is_valid = lambda s: s.isalnum() or s == "_"

    return "".join([
        e if is_valid(e) else "_"
        for e in name.elems()
    ])

def _parsed_string_select(choice):
    """
    Parse a stringified-select where the values are strings

    see
    https://github.com/bazelbuild/bazel/issues/8419
    """
    entries = sorted(choice.removeprefix("select({").removesuffix("})").split(","))

    parse_entry = lambda entry: tuple([
        # remove trailing and leading double quote, fix backslash
        e.strip(" ")[1:-1].replace("\\", "")
        for e in entry.split(": ")
    ])

    return [
        parse_entry(entry)
        for entry in entries
    ]

def stdlib_redirect_library(*, name, source, target, **kwargs):
    """
    Defines a `cc_library` target that redirectes to a different header.

    Creates a single header `cc_library` target that uses a generated
    redirection header. This is typically used to allow targets that depend on
    the C++ standard library using one header include style and remap to a
    different include style.

    For example, some targets in `//arene/base` use the following include
    style:

    ```.cpp
    #include "arene/base/stdlib_choice/enable_if.hpp"
    ```

    This macro provides a mechanism for
    `"arene/base/stdlib_choice/enable_if.hpp"` to redirect to:
    * `<type_traits>`: if using the toolchain provided standard library
    * `"stdlib/include/stdlib_detail/enable_if.hpp"`: if using the arene-base
      provided standard library

    Args:
      name: string
        Name for the target.
      source: string
        Base name of the library header. Use the `include_prefix` attribute to
        specify directories.
      target: string, configurable
        Path of the target header. Allows use of directories. Double quotes
        need to be escaped.
      **kwargs:
        Additional kwargs passed to the `cc_library` target.
    """
    if (type(target) != "select"):
        target = select({
            "//conditions:default": target,
        })

    include_guard = {
        "@INCLUDE_GUARD@": "INCLUDE_GUARD_ARENE_BASE_{}_HPP_".format(
            _replace_invalid_characters(native.package_name() + "_" + source).upper(),
        ),
    }

    strip_include_prefix = {}
    hdrs = {}

    # To handle redirecting with multiple configurations in a `select`, and
    # prevent accidental inclusion of a header from a different configuration,
    # a header needs to be generated in a separate directory for each
    # configuration.
    for i, (cfg, tgt) in enumerate(_parsed_string_select(str(target))):
        directory = "redirect.{}".format(i)
        header_name = "{name}.redirect.{i}".format(
            i = i,
            name = name,
        )

        expand_template(
            name = header_name,
            out = "{directory}/{source}".format(
                directory = directory,
                source = source,
            ),
            substitutions = include_guard | {
                "@TARGET@": tgt,
            },
            template = Label(":redirect.hpp.in"),
            tags = ["manual"],
            visibility = ["//visibility:private"],
        )

        strip_include_prefix |= {cfg: directory}
        hdrs |= {cfg: [header_name]}

    kwargs.setdefault("tags", ["manual"])
    kwargs.setdefault("visibility", ["//visibility:private"])

    cc_library(
        name = name,
        hdrs = select(hdrs),
        strip_include_prefix = select(strip_include_prefix),
        **kwargs
    )
