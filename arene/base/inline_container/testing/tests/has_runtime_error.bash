#!/usr/bin/env bash

# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set -euo pipefail

bin="$1"
msg="$2"

out=$(mktemp)

(./"$bin" 2>&1 || true) >"$out"

echo ""
echo "message: \"$msg\""
echo ""
echo "output of ./$bin:"
cat "$out"
echo ""

grep -q "$msg" "$out"
