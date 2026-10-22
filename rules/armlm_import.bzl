# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Rules that import an armclang license and check that it is still usable.

`//tools/armlm:license.bash.in` is a bash template used by all rules: it
imports a transfer file with `armlm import`, inspects what it got, and fails
unless some product in it is still valid. Each rule expands that template for
its own target, because the paths it needs -- `armlm`, `jq`, and the directory
to import into -- are not the same in a build action as they are under `bazel
run` or `bazel test`.

  armlm_import        imports into a declared output directory, which armclang
                      then checks its licenses out of.
  armlm_license_check a runnable gate, importing into a directory it discards.
  armlm_license_test  one target per case: runs that same gate and asserts its
                      exit status and output.
"""

load("@bazel_skylib//lib:shell.bzl", "shell")

visibility("//...")

_ARMLM_ATTRS = {
    "_armlm": attr.label(
        cfg = "exec",
        default = "@armlm",
        executable = True,
    ),
    "_jq": attr.label(
        cfg = "exec",
        default = "@jq",
        executable = True,
    ),
    "_template": attr.label(
        allow_single_file = True,
        default = "//tools/armlm:license.bash.in",
    ),
}

def _expand_license_script(ctx, *, output, armlm, jq, store, report):
    """Writes the per-caller copy of license.bash.in.

    Args:
      ctx: the rule context.
      output: File the expanded script is written to.
      armlm: path of the `armlm` binary, relative to the directory the script
        runs in -- the execroot in a build action, the runfiles tree under
        `bazel run` and `bazel test`.
      jq: path of the `jq` binary, likewise.
      store: directory `armlm` imports into, or "" to import into a temporary
        directory the script throws away.
      report: True to list every licensed product, False to report only the
        problems.
    """

    # Single quoted in the template, so a quote is the only way out -- and since
    # Bazel builds these paths, one would be a bug here, not a BUILD mistake.
    for path in [armlm, jq, store]:
        if "'" in path:
            fail("path {} cannot be substituted into a quoted string".format(
                repr(path),
            ))

    ctx.actions.expand_template(
        template = ctx.file._template,
        output = output,
        is_executable = True,
        substitutions = {
            "@ARMLM@": armlm,
            "@JQ@": jq,
            "@REPORT@": "1" if report else "0",
            "@STORE@": store,
        },
    )

def _runnable_script_runfiles(ctx, script):
    """Runfiles for a target whose executable is an expanded license script."""
    return ctx.runfiles(
        files = [script, ctx.executable._armlm, ctx.executable._jq],
    ).merge_all([
        ctx.attr._armlm[DefaultInfo].default_runfiles,
        ctx.attr._jq[DefaultInfo].default_runfiles,
    ])

def _import_impl(ctx):
    # A directory, because armlm hashes the license into a file name of its own
    # choosing. It is the real output, not a stamp saying the import happened:
    # armclang checks the license out of it on every compile, through the
    # ARMLM_CACHED_LICENSES_LOCATION below.
    store = ctx.actions.declare_directory(ctx.label.name + "/.armlm")
    script = ctx.actions.declare_file(ctx.label.name + "/import.bash")

    _expand_license_script(
        ctx,
        output = script,
        # An action runs in the execroot.
        armlm = ctx.executable._armlm.path,
        jq = ctx.executable._jq.path,
        store = store.path,
        # Bazel shows a successful action's stderr and drops its stdout, so an
        # inventory here would be invisible; the warnings and the failures are
        # the parts worth keeping.
        report = False,
    )

    ctx.actions.run(
        executable = script,
        outputs = [store],
        # The transfer file lives in $HOME, where Bazel cannot see it change, so
        # rotating the license would otherwise be a silent no-op.
        # `//tools/cache_bust` changes the action key every invocation to force
        # the re-import; see that package for why `no-cache` alone would not.
        #
        # Watching the path from a repository rule would fix invalidation but not
        # caching, and caching is the problem: whether a license is still valid
        # is a function of wall-clock time, which no action key can express. A
        # cached store stops printing the expiry warning -- Bazel shows the
        # stderr only of actions it runs -- and eventually holds an expired
        # license, resurfacing as the L9555E this check replaces. It would also
        # put key material in the action cache.
        #
        # Re-running unconditionally is also why this is one action, not the two
        # //aspects:cpptest.bzl splits a failing analysis into: an import costs
        # about 0.2s and is never worth replaying.
        inputs = [ctx.file._cache_bust],
        # `files_to_run`, so that whatever `armlm` and `jq` need alongside
        # themselves comes with them.
        tools = [
            script,
            ctx.attr._armlm[DefaultInfo].files_to_run,
            ctx.attr._jq[DefaultInfo].files_to_run,
        ],
        use_default_shell_env = True,
        # Keep the per-invocation keys out of both caches, where nothing would
        # reuse them. `armlm import` is deterministic, so an unchanged transfer
        # file yields identical bytes and nothing downstream rebuilds.
        execution_requirements = {
            "no-cache": "1",
            "no-remote-cache": "1",
        },
        mnemonic = "ArmlmImport",
        progress_message = "Importing the armclang license",
    )

    return [
        DefaultInfo(
            # The store, not the script that produced it: a second default
            # output would land in the action key of every armclang compile.
            files = depset([store]),
            executable = store,
        ),
        RunEnvironmentInfo(
            environment = {
                "ARMLM_CACHED_LICENSES_LOCATION": store.path,
            },
        ),
    ]

def _check_impl(ctx):
    _expand_license_script(
        ctx,
        output = ctx.outputs.executable,
        # `bazel run` starts in the runfiles tree.
        armlm = ctx.executable._armlm.short_path,
        jq = ctx.executable._jq.short_path,
        store = "",
        report = True,
    )

    return DefaultInfo(
        runfiles = _runnable_script_runfiles(ctx, ctx.outputs.executable),
    )

def _test_impl(ctx):
    # A sibling of the test executable, not a file under it: the executable is
    # named for the target, so a directory of that name would collide.
    script = ctx.actions.declare_file(ctx.label.name + ".license.bash")

    _expand_license_script(
        ctx,
        output = script,
        # `bazel test` starts in the runfiles tree.
        armlm = ctx.executable._armlm.short_path,
        jq = ctx.executable._jq.short_path,
        store = "",
        report = True,
    )

    ctx.actions.expand_template(
        template = ctx.file._test_template,
        output = ctx.outputs.executable,
        is_executable = True,
        substitutions = {
            "@ENV@": "\n".join([
                "export {}={}".format(name, value)
                for name, value in ctx.attr.env.items()
            ]),
            "@EXPECTED@": "\n  ".join([
                shell.quote(substring)
                for substring in ctx.attr.has_substrs
            ]),
            "@LICENSE_SCRIPT@": shell.quote(script.short_path),
            "@RETURN_CODE@": str(ctx.attr.expected_return_code),
        },
    )

    return [
        DefaultInfo(runfiles = _runnable_script_runfiles(ctx, script)),
        RunEnvironmentInfo(inherited_environment = ctx.attr.env_inherit),
    ]

armlm_import = rule(
    implementation = _import_impl,
    attrs = _ARMLM_ATTRS | {
        "_cache_bust": attr.label(
            allow_single_file = True,
            default = "//tools/cache_bust",
        ),
    },
    # `executable` only so that RunEnvironmentInfo is allowed: Bazel rejects it
    # on a rule that is neither executable nor a test, and
    # //toolchains:armv8-r_toolchain_config reads the store path out of it.
    # Running the target does nothing.
    executable = True,
    doc = """
    Import an armclang license from a transfer file into a store directory.

    The store is the rule's only output and is what
    `ARMLM_CACHED_LICENSES_LOCATION` points armclang at, so it is the artifact
    every armclang compile depends on. Fails the build when every product in the
    transfer file has expired, and warns on stderr when one is about to.
    """,
)

armlm_license_check = rule(
    implementation = _check_impl,
    attrs = _ARMLM_ATTRS,
    executable = True,
    doc = """
    A runnable check that the armclang license is still usable.

    Imports the transfer file into a temporary directory, reports every product
    it holds, and exits non-zero unless one of them is still valid. Runs the
    same script `armlm_import` runs as a build action, so the gate and the build
    cannot disagree about whether the license works.
    """,
)

armlm_license_test = rule(
    implementation = _test_impl,
    attrs = _ARMLM_ATTRS | {
        "env": attr.string_dict(
            doc = """
            Environment to run the check with.

            Unlike the `env` of a native test rule, values are evaluated by the
            shell, so that a case can compute one -- `$(date -u +%s)`, for
            instance. They are expanded into the generated test script rather
            than passed through `RunEnvironmentInfo`, which would pass them
            verbatim.
            """,
        ),
        "env_inherit": attr.string_list(
            doc = "Environment variables to inherit from the invoking shell.",
        ),
        "expected_return_code": attr.int(
            default = 0,
            doc = "Return code the check must exit with. Defaults to `0`.",
        ),
        "has_substrs": attr.string_list(
            doc = "Substrings the combined stdout and stderr must contain.",
        ),
        "_test_template": attr.label(
            allow_single_file = True,
            default = "//tools/armlm:license_test.bash.in",
        ),
    },
    test = True,
    doc = """
    Assert what `armlm_license_check` reports for one license.

    Runs the same script the gate runs, with the same defaults, and checks its
    exit status and output against `expected_return_code` and `has_substrs`. One
    target per case, so a failure names the case that failed.
    """,
)
