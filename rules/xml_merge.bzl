# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Merges several XML documents into a single document.

Naming the documents in a wrapper lets `xmllint` resolve them into one with
XInclude, and the result is well-formed XML.

https://www.w3.org/TR/xinclude-11/#include_element
"""

load("@bazel_skylib//lib:shell.bzl", "shell")

visibility("//...")

# The attributes `xml_merge_action` needs, to be merged into the attributes of a
# rule that calls it.
XML_MERGE_ATTRS = {
    "_xmllint": attr.label(
        default = "@xmllint",
        executable = True,
        cfg = "exec",
    ),
}

# The wrapper is piped in rather than written to a file, so an 'href' resolves
# against the working directory of the action and every path stays relative.
#
# '--nofixup-base-uris' keeps `xmllint` from recording the path of each document
# in an 'xml:base' attribute of the result.
# https://man.archlinux.org/man/xmllint.1.en
_COMMAND = """
set -euo pipefail

(
  echo '<?xml version="1.0" encoding="UTF-8"?>'
  echo '<{root} xmlns:xi="http://www.w3.org/2001/XInclude">'

  for path in {paths}; do
    if [ -d "$path" ]; then
      for xml in "$path"/*.xml; do
        echo "<xi:include href=\\"$xml\\"/>"
      done
    else
      echo "<xi:include href=\\"$path\\"/>"
    fi
  done

  echo '</{root}>'
) | {xmllint} --nofixup-base-uris --xinclude - -o {output}
"""

def xml_merge_action(ctx, *, srcs, output, root):
    """Declares an action merging `srcs` into `output`.

    The calling rule must include `XML_MERGE_ATTRS` in its attributes.

    Args:
      ctx: ctx
        Context of the calling rule.
      srcs: list[File]
        Documents to merge. A directory contributes every `*.xml` it holds.
      output: File
        The merged document.
      root: string
        Name of the element holding the merged documents.
    """
    ctx.actions.run_shell(
        inputs = srcs,
        outputs = [output],
        tools = [ctx.executable._xmllint],
        use_default_shell_env = True,
        mnemonic = "XmlMerge",
        progress_message = "Merging XML documents for %{label}",
        command = _COMMAND.format(
            output = shell.quote(output.path),
            paths = " ".join([shell.quote(src.path) for src in srcs]),
            root = root,
            xmllint = shell.quote(ctx.executable._xmllint.path),
        ),
    )

def _xml_merge_impl(ctx):
    output = ctx.actions.declare_file("{}.xml".format(ctx.label.name))

    xml_merge_action(
        ctx,
        srcs = ctx.files.srcs,
        output = output,
        root = ctx.attr.root,
    )

    return [DefaultInfo(files = depset([output]))]

xml_merge = rule(
    implementation = _xml_merge_impl,
    attrs = XML_MERGE_ATTRS | {
        "root": attr.string(
            default = "all",
            doc = "Name of the element holding the merged documents.",
        ),
        "srcs": attr.label_list(
            allow_files = True,
            doc = "Documents to merge. A directory contributes every `*.xml` it holds.",
        ),
    },
    doc = """
Merges every document in `srcs` into `<name>.xml`.

```starlark
xml_merge(
    name = "compounds",
    srcs = [":docs.xml"], # directory containing XML files
 )
 xml_merge(
   name = "aggregate",
   srcs = [":report1", ":report2"],
)
```
""",
)
