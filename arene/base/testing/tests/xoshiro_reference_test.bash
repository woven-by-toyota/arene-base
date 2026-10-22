#!/usr/bin/env bash

# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set -euo pipefail

readonly expected_dump="$1"
readonly actual_dump="$2"

cmp \
  <("$expected_dump" "$SEED_U64" | head -c "$TEST_LENGTH") \
  <("$actual_dump" "$SEED_U64" | head -c "$TEST_LENGTH")
