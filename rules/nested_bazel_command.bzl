# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Runnable target that invokes a nested call to Bazel.

These are typically used with runnable Bazel targets that use wildcard patterns.
"""

visibility("//...")

def bazel_command(
        *,
        name,
        command,
        startup_options = [],
        command_options = [],
        default_target = [
            "--",
            "//...",
        ],
        **kwargs):
    """Runnable target that will invoke `bazelisk`.

    Args:
      name: string
        Name of the runnable target.
      command: string
        Bazel command to invoke (e.g. `build`).
      startup_options: string_list (default: `[]`)
        Options to pass as a startup options to the Bazel command.
      command_options: string_list (default: `[]`)
        Options to pass to the Bazel command.
      default_target: string_list (default: `["//..."]`)
        Target if none are specified when running `name`.
      **kwargs:
        Additional kwargs passed to the underlying `genrule` target.
    """
    native.genrule(
        name = name,
        outs = [name + ".bash"],
        cmd = """
echo "#!/usr/bin/env bash" > $@
echo "set -euo pipefail" >> $@
echo "cd \\$$BUILD_WORKSPACE_DIRECTORY" >> $@
echo "exec bazelisk {startup_options} {command} {command_options} \\$${{@:-{default_target}}}" >> $@
    """.format(
            command = command,
            startup_options = " ".join(startup_options),
            command_options = " ".join(command_options),
            default_target = " ".join(default_target),
        ),
        executable = True,
        **kwargs
    )

def bazel_build(
        *,
        name,
        build_options = [],
        **kwargs):
    """Runnable target that will invoke `bazelisk build`.

    Args:
      name: string
        Name of the runnable target.
      build_options: string_list (default: `[]`)
        Options to pass to the Bazel command.
      **kwargs:
        Additional kwargs passed to `bazel_command`.
    """
    if "command_options" in kwargs.keys():
        fail("use `build_options` instead of `command_options` with `bazel_build`")

    bazel_command(
        name = name,
        command = "build",
        command_options = build_options,
        **kwargs
    )
