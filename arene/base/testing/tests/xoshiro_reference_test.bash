#!/usr/bin/env bash
set -euo pipefail

readonly expected_dump="$1"
readonly actual_dump="$2"

cmp \
  <("$expected_dump" "$SEED_U64" | head -c "$TEST_LENGTH") \
  <("$actual_dump" "$SEED_U64" | head -c "$TEST_LENGTH")
