#!/usr/bin/env bash

# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set -euo pipefail

# Script to extract the public header for a single component in
# stdlib_detail.
#
# The public header for a component is either:
# 1. a public header in the standard library (e.g. <type_traits>)
# 2. nothing (e.g. for headers that only provide implementation details)
#
# Based on these preconditions, this script *should* be able to emit the
# component's public header mapping. This mapping is used to define the dispatch
# header in stdlib_choice.
#

arg="$1"

public="$(sed -n -e 's/.*IWYU pragma: private, include <\(.*\)>/\"\1\"/p' "$arg")"
base=$(basename "$arg")

echo "struct("
echo "    name = \"${base%.hpp}\","
echo "    public_header = ${public:-None},"
echo "),"
