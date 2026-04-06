# Copyright 2019 The Bazel Authors. All rights reserved.
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""A test rule that compares two binary files.

The rule uses a Bash command diff.

It is largely copied from the upstream source at:
  https://github.com/bazelbuild/bazel-skylib/tree/main

Modifications from upstream include:
  - ignore trailing carriage returns when diffing files
  - use color to improve diagnostics on failure
"""

load("@bazel_skylib//lib:shell.bzl", "shell")

visibility("//...")

def _runfiles_path(f):
    if f.root.path:
        return f.path[len(f.root.path) + 1:]  # generated file
    else:
        return f.path  # source file

def _diff_test_impl(ctx):
    test_bin = ctx.actions.declare_file(ctx.label.name + "-test.sh")
    ctx.actions.write(
        output = test_bin,
        content = r"""#!/usr/bin/env bash
set -euo pipefail
F1="{file1}"
F2="{file2}"
[[ "$F1" =~ ^external/* ]] && F1="${{F1#external/}}" || F1="$TEST_WORKSPACE/$F1"
[[ "$F2" =~ ^external/* ]] && F2="${{F2#external/}}" || F2="$TEST_WORKSPACE/$F2"
if [[ -d "${{RUNFILES_DIR:-/dev/null}}" && "${{RUNFILES_MANIFEST_ONLY:-}}" != 1 ]]; then
  RF1="$RUNFILES_DIR/$F1"
  RF2="$RUNFILES_DIR/$F2"
elif [[ -f "${{RUNFILES_MANIFEST_FILE:-/dev/null}}" ]]; then
  RF1="$(grep -F -m1 "$F1 " "$RUNFILES_MANIFEST_FILE" | sed 's/^[^ ]* //')"
  RF2="$(grep -F -m1 "$F2 " "$RUNFILES_MANIFEST_FILE" | sed 's/^[^ ]* //')"
elif [[ -f "$TEST_SRCDIR/$F1" && -f "$TEST_SRCDIR/$F2" ]]; then
  RF1="$TEST_SRCDIR/$F1"
  RF2="$TEST_SRCDIR/$F2"
else
  echo >&2 "ERROR: could not find \"{file1}\" and \"{file2}\""
  exit 1
fi
if ! {diff} "$RF1" "$RF2"; then
  echo >&2 "FAIL: files \"{file1}\" and \"{file2}\" differ. "{fail_msg}
  exit 1
fi
""".format(
            diff = ctx.attr.diff,
            fail_msg = shell.quote(ctx.attr.failure_message),
            file1 = _runfiles_path(ctx.file.file1),
            file2 = _runfiles_path(ctx.file.file2),
        ),
        is_executable = True,
    )
    return DefaultInfo(
        executable = test_bin,
        files = depset(direct = [test_bin]),
        runfiles = ctx.runfiles(files = [test_bin, ctx.file.file1, ctx.file.file2]),
    )

diff_test = rule(
    doc = """A test that compares two files.

    The test succeeds if the files' contents match.
    """,
    attrs = {
        "file1": attr.label(
            doc = "Label of the file to compare to `file2`.",
            allow_single_file = True,
            mandatory = True,
        ),
        "file2": attr.label(
            doc = "Label of the file to compare to `file1`.",
            allow_single_file = True,
            mandatory = True,
        ),
        "failure_message": attr.string(
            doc = "Additional message to log if the files' contents do not match.",
        ),

        # SPDX-SnippetBegin
        # SPDX-SnippetCopyrightText: 2026, Toyota Motor Corporation
        "diff": attr.string(
            default = "diff --color=always",
            doc = "diff command to use",
        ),
        # SPDX-SnippetEnd
    },
    test = True,
    implementation = _diff_test_impl,
)
