# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Provides a rule that exposes all transitively reachable source files.
"""

visibility("//...")

SourceFilesInfo = provider(
    doc = "Transitive set of C++ source files",
    fields = {
        "files": "depset[File]: source files",
    },
)

# Attributes the aspect propagates along AND collects source files from.
# Must include any attribute used by wrapper/transition rules in the dep
# graph (e.g. `transition_use_toolchain_stdlib` uses `src`), otherwise the
# aspect stops at the wrapper and the testonly subtree below is invisible.
_DEP_ATTRS = {
    "deps": lambda x: x,
    "src": list,
}

def _source_files_aspect_impl(_, ctx):
    """Collects source/header files belonging to non-`testonly` targets.

    These files are tracked so that rules (e.g. coverage, doxygen) can access
    source files when written hermetically.
    """
    direct = []
    if not ctx.rule.attr.testonly:
        for attr in ["hdrs", "srcs"]:
            direct.extend(getattr(ctx.rule.files, attr, []))

    transitive = []
    for attr, to_list in _DEP_ATTRS.items():
        value = getattr(ctx.rule.attr, attr, None)
        if value == None:
            continue

        for dep in to_list(value):
            if SourceFilesInfo in dep:
                transitive.append(dep[SourceFilesInfo].files)

    return [SourceFilesInfo(
        files = depset(
            direct = direct,
            transitive = transitive,
        ),
    )]

source_files_aspect = aspect(
    implementation = _source_files_aspect_impl,
    attr_aspects = _DEP_ATTRS.keys(),
    required_providers = [CcInfo],
)

def _source_files_impl(ctx):
    transitive = [
        dep[SourceFilesInfo].files
        for dep in ctx.attr.srcs
        if SourceFilesInfo in dep
    ]
    sources = depset(transitive = transitive)

    return [
        DefaultInfo(
            runfiles = ctx.runfiles(
                transitive_files = sources,
            ),
        ),
        SourceFilesInfo(files = sources),
    ]

source_files = rule(
    implementation = _source_files_impl,
    attrs = {
        "srcs": attr.label_list(
            aspects = [source_files_aspect],
            providers = [CcInfo],
            doc = "C++ targets whose transitive sources should be collected.",
        ),
    },
    doc = """
Collects all C++ source files transitively reachable from `srcs`.

Outputs:
  - DefaultInfo.runfiles: the actual source files, available to consumers.
  - SourceFilesInfo: the depset of File objects, for further composition.
    """,
)
