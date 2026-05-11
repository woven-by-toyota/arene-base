# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Workaround for obtaining the plugin headers for a GCC toolchain.

This is a workaround that bypasses the need for plugin headers to be
specified as `cxx_builtin_directories` in the toolchain config.
"""

load(
    "@rules_cc//cc:find_cc_toolchain.bzl",
    "CC_TOOLCHAIN_TYPE",
    "find_cc_toolchain",
)

def _gcc_plugin_headers_impl(ctx):
    cc_toolchain = find_cc_toolchain(ctx)

    include_dir = ctx.actions.declare_directory(ctx.label.name)

    ctx.actions.run_shell(
        inputs = cc_toolchain.all_files.to_list(),
        outputs = [include_dir],
        command = """
cp -R "$({gcc} -print-file-name=plugin/include)"/* {include_dir}

# also provide gmp headers if we can find them
gmp_incdir=$({gcc} -v 2>&1  | tr ' ' '\\n' | grep -- '^--with-gmp=' | cut -d= -f2)
[[ -z "$gmp_incdir" ]] || cp -R "$gmp_incdir"/include/* {include_dir}
        """.format(
            gcc = cc_toolchain.compiler_executable,
            include_dir = include_dir.path,
        ),
        use_default_shell_env = True,
    )

    files = depset([include_dir])

    return [
        DefaultInfo(files = files),
        CcInfo(
            compilation_context = cc_common.create_compilation_context(
                # Even though these copied headers will be included with
                # `-isystem`, we need to specify them in `headers` so that Bazel
                # will actually build these.
                headers = files,
                system_includes = depset([include_dir.path]),
            ),
        ),
    ]

gcc_plugin_headers = rule(
    implementation = _gcc_plugin_headers_impl,
    toolchains = [CC_TOOLCHAIN_TYPE],
)
