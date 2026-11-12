#!/usr/bin/env bash

# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set -euo pipefail

readonly rng_test="$1"
output="$(mktemp)"

"$rng_test" --self_test | tee "$output"
grep -q "self-test passed" "${output}"
