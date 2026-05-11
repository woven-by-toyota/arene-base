#!/usr/bin/env bash

# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set -euo pipefail

# Script to generate a snippet of a BUILD file for a single component in
# stdlib_detail.
#
# The include statements are constrained to either:
# 1. other components in stdlib_detail
# 2. exposed components in arene/base
#
# Based on these preconditions, this script *should* be able to emit the
# component's cc_library target with the correct dependencies.
#
# One dependency is generated for each included file, with the dependency's name
# matching that of the included file without its extension.
#
# Additionally, arbitrary dependencies can be provided by using the comment
# format:
# // bazel_dep {target}

arg="$1"

BAZEL_DEP_PREFIX='// bazel_dep '

# a component may have no include statements and that is fine
readarray -t includes < <(grep -E '^#include ".*\.hpp"' "$arg" || true)

# a component may have no additional deps that is also fine
readarray -t extra_deps < <(grep -E "$BAZEL_DEP_PREFIX" "$arg" || true)

# determine the header path from an include statement
function header {
  local strip_prefix
  strip_prefix="${1#\#include \"}"
  echo "${strip_prefix%\"*}"
}

# determine a Bazel label based on a header path
#
# If the path starts with 'stdlib/include/stdlib_detail', then the package is
# omitted.
function target {
  if [[ "$1" == stdlib/include/stdlib_detail* ]]; then
    local name
    name="$(basename "$1")"
    echo "${name%.hpp}"
  else
    local name
    local package
    name="$(basename "$1")"
    package="$(dirname "$1")"
    echo "//${package}:${name%.hpp}"
  fi
}

# extract the label of a bazel dep
function dep_label {
  local strip_prefix
  strip_prefix="${1#"$BAZEL_DEP_PREFIX"}"
  echo "$strip_prefix"
}

indent="    "

echo "stdlib_detail_component_library("
echo "${indent}name = \"$(target "$arg")\","
echo "${indent}visibility = [\"//:__subpackages__\"],"

if ((${#includes[@]} + ${#extra_deps[@]})); then
  echo "${indent}deps = ["
  for line in "${includes[@]}"; do
    dep="$(target "$(header "$line")")"
    if [[ "$dep" != //* ]]; then
      dep=":${dep}"
    fi
    echo "${indent}${indent}\"${dep}\","
  done

  for dep in "${extra_deps[@]}"; do
    echo "${indent}${indent}\"$(dep_label "$dep")\","
  done

  echo "${indent}],"
fi

echo ")"
echo ""
