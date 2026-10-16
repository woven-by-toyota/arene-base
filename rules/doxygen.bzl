# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""Generates Doxygen documentation.

Wraps the rule from `@rules_doxygen` with the configuration shared by every
Doxygen target in this repository, so that a target only declares what differs.

```
load("//rules:doxygen.bzl", "doxygen")

doxygen(
    name = "test_specification",
    srcs = ["test_report_layout"],
    layout_file = "tools/test_specification/test_report_layout",
    project_name = "Arene Base Test Specification",
)
```

see:
https://github.com/TendTo/rules_doxygen/blob/main/docs/doxygen_doc.md
https://www.doxygen.nl/manual/config.html
"""

load("@rules_doxygen_ext//:doxygen.bzl", _doxygen = "doxygen")

visibility("//...")

_DEFAULTS = {
    "aliases": [
        "woven=Woven by Toyota",
        "the_team=Vehicle SDK Team",
        "arene_base=@c arene-base",
    ],
    "builtin_stl_support": True,
    "case_sense_names": "YES",
    "distribute_group_doc": True,
    "exclude_symbols": [
        "*::detail",
        "*::*_detail",
        "*::*_impl",
    ],
    "expand_only_predef": True,
    "extract_local_classes": False,
    "generate_buglist": False,
    "generate_html": True,
    "generate_latex": False,
    "generate_todolist": False,
    "group_nested_compounds": True,
    "have_dot": False,  # reduce the HTML size
    "html_extra_stylesheet": [
        # `doxygen-awesome-sidebar-only.css` clamps `#top` to the sidebar width and
        # hides the overflow, which clips the static tab bar when there is no
        # treeview to fill that column. We only use the base theme to avoid sidebar
        # nav, which doesn't work with Artifactory hosting.
        "docs/doxygen-awesome.css",
    ],
    "macro_expansion": True,
    "markdown_id_style": "GITHUB",
    "num_proc_threads": 0,  # one thread per core
    "predefined": [
        "__attribute__(x)=",
        "decltype(x)=",
        "ARENE_DEPRECATED_WITH(x)=",
        "ARENE_DOC_ONLY(n)=n",
        "ARENE_DOC_GENERATION_RUNNING=",
        "ARENE_IGNORE_START()=",
        "ARENE_IGNORE_END()=",
        "ARENE_IGNORE_ALL(f, r)=",
        "ARENE_IGNORE_CLANG(f, r)=",
        "ARENE_IGNORE_GCC(f, r)=",
        "ARENE_IGNORE_ARMCLANG(f, r)=",
    ],
    "project_brief": "Fundamental Utilities For Safety Critical C++",
    "project_logo": "docs/images/arene-os-logo.png",
    "sort_brief_docs": True,
    "sort_members_ctors_1st": True,
    "use_default_shell_env": True,
    "verbatim_headers": False,  # reduce the HTML size
    "warn_as_error": "FAIL_ON_WARNINGS",
    "warn_if_doc_error": True,
    "warn_if_incomplete_doc": True,
    "warn_if_undocumented": True,
    "warn_no_paramdoc": True,
    "warnings": True,
    # Repeat members at namespace scope additionally at file scope. This
    # simplifies discovery of `@test`.
    "xml_ns_memb_file_scope": True,
    # Disable program listings by default to reduce output size (e.g. syntax
    # highlighting, cross-ref info)
    "xml_programlisting": False,
}

# default srcs that are always available to the rule
DEFAULT_SRCS = [
    "//docs:doxygen-awesome.css",
    "//docs/images:arene-os-logo.png",
]

# output formats:
# https://www.doxygen.nl/manual/config.html
OUTPUTS = [
    "generate_docbook",
    "generate_html",
    "generate_latex",
    "generate_man",
    "generate_rtf",
    "generate_xml",
]

def doxygen(
        name,
        *,
        srcs = [],
        tags = None,
        visibility = None,
        **kwargs):
    """Generates Doxygen documentation

    Generates documentation in the formats specified by `generate_*` attrs,
    and each is provided by `<name>.<format>` as a single directory so that it
    can be used with `$(rootpath)`. By default, `generate_html` is `True` and
    all others are `False`.

    ```
    doxygen(name = "docs")                        # docs.html
    doxygen(name = "docs", generate_xml = True)   # docs.html and docs.xml
    ```

    Args:
      name: string
        Name of the target.
      srcs: label_list (default `[]`)
        Sources to document, appended to `DEFAULT_SRCS`.
      tags: string_list (default `None`)
        Tags of the output targets.
      visibility: label_list (default `None`)
        Visibility of the output targets.
      **kwargs:
        Additional named arguments to pass to `@rules_doxygen`'s `doxygen`
        macro, overriding anything the wrapper sets (e.g. `generate_xml`,
        `project_name`, `exclude_patterns`). The `*_output` attrs are
        rejected, since the outputs are set by this macro.
    """
    doxygen_target = "{}_doxygen".format(name)
    attrs = _DEFAULTS | kwargs

    for attr in attrs:
        if attr.endswith("_output"):
            fail("`{}` is not supported, the outputs are set by this macro".format(
                attr,
            ))

    # Nest both the generated Doxyfile and the outputs under `name` so that two
    # `doxygen` targets in the same package cannot collide.
    # `outs` is declared relative to the package, while `OUTPUT_DIRECTORY` is
    # taken from the directory holding the generated Doxyfile. Neither depends
    # on the target name.
    #
    # https://github.com/TendTo/rules_doxygen/blob/a060c237d81cb81a24c4e8f6493422af2099cfa1/doxygen/doxygen.bzl#L64
    # https://github.com/TendTo/rules_doxygen/blob/a060c237d81cb81a24c4e8f6493422af2099cfa1/doxygen/doxygen.bzl#L59
    # https://github.com/TendTo/rules_doxygen/blob/a060c237d81cb81a24c4e8f6493422af2099cfa1/doxygen/doxygen.bzl#L82
    # https://github.com/TendTo/rules_doxygen/blob/a060c237d81cb81a24c4e8f6493422af2099cfa1/doxygen/doxygen.bzl#L146
    outs = {
        format: "{}/{}".format(name, format)
        for format in [
            attribute.split("_")[1]
            for attribute in OUTPUTS
            if attrs.get(attribute)
        ]
    }

    if not outs:
        fail("no output is generated, at least one `generate_*` attribute must be set")

    _doxygen(
        name = doxygen_target,
        srcs = DEFAULT_SRCS + srcs,
        tags = ["manual"],
        visibility = ["//visibility:private"],
        **(attrs | {
            "outs": outs.values(),
            "doxyfile_prefix": name,
        })
    )

    for format, out_relpath in outs.items():
        native.filegroup(
            name = "{}.{}".format(name, format),
            srcs = [doxygen_target],
            output_group = out_relpath,
            tags = tags,
            visibility = visibility,
        )

    # Don't include the generated Doxyfile, otherwise dependent rules that
    # expect a single output (when generated with a single output) will fail.
    native.filegroup(
        name = name,
        srcs = ["{}.{}".format(name, format) for format in outs],
        tags = tags,
        visibility = visibility,
    )
