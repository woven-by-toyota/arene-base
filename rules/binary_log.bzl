# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Provides a rule that gets the output of running a binary as a build target.
"""

load("@bazel_skylib//lib:shell.bzl", "shell")
load("@platforms//host:constraints.bzl", "HOST_CONSTRAINTS")

visibility("//...")

def binary_log(
        *,
        name,
        src,
        out = None,
        file_descriptor = "stdout",
        return_code = 0,
        env = {},
        cfg = "target",
        run_under = None,
        **kwargs):
    """
    Macro to capture binary output when running an executable and write it to a log.

    NOTE: This output may be missing from the local machine if the result is
    stored in the remote cache. For example

    ```sh
    bazel build //testlibs/minitest/tests:panic_example_out
    cat bazel-out/k8-fastbuild/bin/testlibs/minitest/tests/panic_example_out.log

    cat: bazel-out/k8-fastbuild/bin/testlibs/minitest/tests/panic_example_out.log: No such file or directory
    ```

    The output can be explictly downloaded with
    ```sh
    bazel build \
      --remote_download_all \
      //testlibs/minitest/tests:panic_example_out
    ```

    For more information, see:
    https://github.com/bazelbuild/bazel/issues/16514

    Args:
      name: string
        Name for this target.
      src: label
        Binary target to run.
      out: string (default `name + ".log")
        Name for the created log file.
      file_descriptor: string|None (default `stdout`)
        File descriptor to log. If `None`, no output is redirected to the log
        file directly. It must be created by executing the binary.
      return_code: int
        Required return code from running the exectuable.
      env: string_dict (default `{}`)
        Specifies additional environment variables to set when running the
        binary. Values are subject to 'make variable' substitution.
      cfg: string (default `"target"`)
        Specify if the log is generated on the target platform or on the machine
        performing the build actions.
        Must be "target" or "exec".
      run_under: label|None
        Runner to use to run the binary. Requires `cfg` to be "exec".
      **kwargs:
        Additional named arguments to pass to `genrule`.
    """
    allowed_fds = {
        "stdout": 1,
        "stderr": 2,
    }
    if not (file_descriptor in allowed_fds.keys() or file_descriptor == None):
        fail("`fd` must be `None` or one of: {}".format(", ".join(allowed_fds.keys())))

    allowed_cfgs = ["target", "exec"]
    if cfg not in allowed_cfgs:
        fail("`cfg` must be one of: {}".format(", ".join(allowed_cfgs)))

    if run_under and cfg != "exec":
        fail("`cfg` must be set to \"exec\" to use `run_under`.")

    if out == None:
        out = name + ".log"

    error_message = shell.quote(
        "ERROR: when running executable for '{}',".format(src) +
        "\ndid not get expected return code {}.".format(return_code),
    )

    redirect = (
        "1> /dev/null" if file_descriptor == None else "{fd}> $@".format(
            fd = allowed_fds[file_descriptor],
        )
    )
    binary = "$(location {})".format(src)

    if cfg == "target":
        srcs = [src]
        tools = None
        target_compatible_with = HOST_CONSTRAINTS
    else:
        srcs = None
        tools = [src]
        target_compatible_with = None
        if run_under:
            binary = "$(location {}) ".format(run_under) + binary
            tools.append(run_under)

    cmd = "; ".join([
        "set -euo pipefail",
        "rc=0",
        "{env} {binary} {redirect} || rc=$$?",
        "[ $$rc -eq {return_code} ] || ( echo {message} && echo got $$rc && echo OUTPUT: && cat $@ && false )",
    ]).format(
        env = " ".join([
            "{}={}".format(key, value)
            for key, value in env.items()
        ]),
        binary = binary,
        redirect = redirect,
        return_code = return_code,
        message = error_message,
    )

    native.genrule(
        name = name,
        srcs = srcs,
        tools = tools,
        outs = [out],
        cmd = cmd,
        target_compatible_with = target_compatible_with,
        **kwargs
    )
