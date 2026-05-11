# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Rule to import a license file.
"""

def _impl(ctx):
    # this path exists and is stable across two users
    store_relpath = "store/1/c60cc4b53606af97f06aae3a415df5919691d323"

    output = ctx.actions.declare_file(ctx.label.name + "/.armlm/" + store_relpath)

    default_transfer_file_path = "~/.config/armclang-transfer-file"

    error_message = "\n".join([
        "\n",
        "If you need to register a license, run this command to create a transfer file:",
        "\n",
        "bazelisk run @armlm -- activate --code <ACTIVATION_CODE> --as-user $(whoami) --to-file {transfer_file} --borrow-period 365",
        "\n",
    ]).format(
        transfer_file = default_transfer_file_path,
    )

    ctx.actions.run_shell(
        command = """
#!/usr/bin/env bash
set -euo pipefail

ln -s {transfer_file} transfer_file

# change the HOME directory so armlm doesn't try to write to a readonly directory
export HOME=.

({armlm} import --file transfer_file && mv .armlm/*/*/* {out}) || (echo '{message}' && false)
        """.format(
            transfer_file = default_transfer_file_path,
            armlm = ctx.expand_location(
                "$(location @armlm//:armlm)",
                targets = [ctx.attr._armlm],
            ),
            out = output.path,
            message = error_message,
        ),
        outputs = [output],
        inputs = ctx.files._armlm,
        use_default_shell_env = True,
    )

    return [
        DefaultInfo(
            executable = output,
        ),
        RunEnvironmentInfo(
            environment = {
                "ARMLM_CACHED_LICENSES_LOCATION": output.path.removesuffix(store_relpath),
            },
        ),
    ]

armlm_import = rule(
    implementation = _impl,
    attrs = {
        "_armlm": attr.label(
            default = "@armlm",
        ),
    },
    # This rule is only `executable` so that we can forward an environment
    # variable
    executable = True,
    doc = """
    Import an Arm Clang license from a transfer file.
    """,
)
