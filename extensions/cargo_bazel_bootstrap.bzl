"""
Module extension to override the cargo-bazel bootstrap linker.

rules_rust's `cargo_bazel_bootstrap` builds the `cargo-bazel` binary, which is
subsequently used to resolve and build crates. During this bootstrap build,
`cargo` links with `cc`, which it expects to find on the PATH.

Because `cargo-bazel` is built inside a repository rule (during the loading
phase), our hermetic toolchains are not available — repository rules execute
before toolchain resolution. The bootstrap must therefore rely on a system
linker.

Some build environments lock down the PATH, causing the bootstrap to fail when
`cc` is not available. To work around this without:

1. Exposing `cc` on the PATH globally
2. Setting `--repo_env=CC=...` which would affect all other repository rules

this extension hard-codes the linker location *only* for the cargo-bazel
bootstrap build.
"""

load("@rules_rust//cargo:defs.bzl", "cargo_env")
load("@rules_rust//crate_universe:deps_bootstrap.bzl", "cargo_bazel_bootstrap")

def _cargo_bootstrap_override_linker_impl(module_ctx):
    deps = cargo_bazel_bootstrap(
        name = "cargo_bazel_bootstrap_override_linker",
        rust_toolchain_cargo_template = "@rust_host_tools//:bin/{tool}",
        rust_toolchain_rustc_template = "@rust_host_tools//:bin/{tool}",
        compressed_windows_toolchain_names = False,
        env = {
            "*": cargo_env({
                "RUSTFLAGS": "-C linker=/usr/bin/cc",
            }),
        },
    )

    return module_ctx.extension_metadata(
        root_module_direct_dev_deps = [repo.repo for repo in deps],
        root_module_direct_deps = [],
    )

cargo_bootstrap_override_linker = module_extension(
    implementation = _cargo_bootstrap_override_linker_impl,
)
