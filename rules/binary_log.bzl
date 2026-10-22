# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Provides a rule that gets the output of running a binary as a build target.
"""

load("@bazel_skylib//lib:shell.bzl", "shell")

visibility("//...")

def binary_log(
        *,
        name,
        src,
        out = None,
        data = [],
        file_descriptor = "stdout",
        return_code = 0,
        env = {},
        run_under = {
            "//configs:host_platform": "",
        },
        **kwargs):
    """
    Macro to capture binary output when running an executable and write it to a log.

    Runs `src` (optionally under a platform-specific runner such as QEMU),
    captures the selected file descriptor, checks the return code, and emits
    the captured output as a build artifact.

    Use this macro when:

    - The binary is built for a **non-host target platform** and must be
        executed via a runner (e.g. QEMU for cortex-r52).
    - You want uniform return-code + fd-capture semantics across host and
        cross-compile builds.

    If you only need to run a binary on the **host platform** (e.g. a code
    generator), prefer a plain `genrule` with the binary under `tools`:

    ```starlark
    genrule(
        name = "generated_src",
        outs = ["generated.cpp"],
        cmd = "$(execpath :my_codegen) > $@",
        tools = [":my_codegen"],
    )
    ```

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
        Binary target to run. Built in the target configuration (so it is
        cross-compiled when `--platforms=<cross-platform>` is used). Must be an
        executable target (e.g. `cc_binary`).
      out: string (default `name` + ".log")
        Name for the created log file.
      data: label_list (default `[]`)
        Files `src` needs to run, located through `env` with `$(execpath)`
        (genrule provides no runfiles). Built in the target configuration.
      file_descriptor: string|None (default `stdout`)
        File descriptor to log. If `None`, no output is redirected to the log
        file directly. It must be created by executing the binary.
      return_code: int (default `0`)
        Required return code from running the exectuable.
      env: string_dict (default `{}`)
        Specifies additional environment variables to set when running the
        binary. Values are subject to 'make variable' substitution.
      run_under: string_dict (unspecified default)
        Selects the runner used to invoke the binary based on the target
        platform. Keys are `config_setting` labels (typically one per
        supported target platform); values are labels to a runner binary, or
        the empty string `""` to invoke `src` directly with no wrapper.

        The runner, when specified, is built in the exec configuration (i.e.
        for the host), while `src` is built in the target configuration. This
        lets you run a cross-compiled binary on the host via, e.g., QEMU.
      **kwargs:
        Additional named arguments to pass to `genrule` (e.g. `tags`,
        `visibility`, `testonly`).
    """
    allowed_fds = {
        "stdout": 1,
        "stderr": 2,
    }
    if not (file_descriptor in allowed_fds.keys() or file_descriptor == None):
        fail("`fd` must be `None` or one of: {}".format(", ".join(allowed_fds.keys())))

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

    cmd = "; ".join([
        "set -euo pipefail",
        "rc=0",
        "{env} {{runner}} {binary} {redirect} || rc=$$?",
        "[ $$rc -eq {return_code} ] || ( echo {message} && echo got $$rc && echo OUTPUT: && cat $@ && false )",
    ]).format(
        env = " ".join([
            "{}={}".format(key, value)
            for key, value in env.items()
        ]),
        binary = "$(location {})".format(src),
        redirect = redirect,
        return_code = return_code,
        message = error_message,
    )

    native.genrule(
        name = name,
        srcs = [src] + data,
        tools = select({
            config: [runner] if runner else []
            for config, runner in run_under.items()
        }),
        outs = [out],
        cmd = select({
            config: cmd.format(
                runner = "$(location {})".format(runner) if runner else "",
            )
            for config, runner in run_under.items()
        }),
        target_compatible_with = select(
            {"//conditions:default": ["@platforms//:incompatible"]} |
            {config: [] for config in run_under.keys()},
        ),
        **kwargs
    )
