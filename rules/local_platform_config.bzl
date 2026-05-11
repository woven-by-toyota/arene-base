# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Define paths used by the local platform.
"""

visibility("//...")

def _is_ubuntu_20_04(rctx):
    """Detect whether the host OS is Ubuntu 20.04.

    Args:
        rctx: The repository context.

    Returns:
        True if the host OS is Ubuntu 20.04, False otherwise.
    """
    result = rctx.execute(["cat", "/etc/os-release"])
    if result.return_code != 0:
        return False

    distro_id = ""
    version_id = ""
    for line in result.stdout.splitlines():
        line = line.strip()
        if line.startswith("ID="):
            distro_id = line.removeprefix("ID=").strip('"')
        elif line.startswith("VERSION_ID="):
            version_id = line.removeprefix("VERSION_ID=").strip('"')

    return distro_id == "ubuntu" and version_id == "20.04"

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

    llvm_toolchain = "@llvm{version}_toolchain//:all".format(
        version = 13 if _is_ubuntu_20_04(rctx) else 20,
    )

    rctx.file(
        "config.bzl",
        content = """
BAZEL_EXTERNAL_DIR = "{external_dir}"
BAZEL_VERSION = "{bazel_version}"
BAZEL_WORKSPACE_ROOT = "{workspace_root}"
DEFAULT_TOOLCHAINS_LLVM_TOOLCHAIN = "{llvm_toolchain}"
""".format(
            bazel_version = native.bazel_version,
            external_dir = str(rctx.path(".").realpath).removesuffix("/" + rctx.name),
            workspace_root = rctx.workspace_root,
            llvm_toolchain = llvm_toolchain,
        ),
        executable = False,
    )

local_platform_config = repository_rule(
    implementation = _local_platform_config_impl,
    local = True,
    configure = True,
    doc = "Obtains paths used by the local platform.",
)
