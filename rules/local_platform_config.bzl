# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Define paths used by the local platform.
"""

visibility("//...")

def _local_platform_config_impl(rctx):
    rctx.file(
        "WORKSPACE.bazel",
        content = "",
        executable = False,
    )

    rctx.file(
        "BUILD.bazel",
        content = "",
        executable = False,
    )
    rctx.file(
        "config.bzl",
        content = """
BAZEL_EXTERNAL_DIR = "{external_dir}"
BAZEL_VERSION = "{bazel_version}"
BAZEL_WORKSPACE_ROOT = "{workspace_root}"
""".format(
            bazel_version = native.bazel_version,
            external_dir = str(rctx.path(".").realpath).removesuffix("/" + rctx.name),
            workspace_root = rctx.workspace_root,
        ),
        executable = False,
    )

local_platform_config = repository_rule(
    implementation = _local_platform_config_impl,
    local = True,
    configure = True,
    doc = "Obtains paths used by the local platform.",
)
