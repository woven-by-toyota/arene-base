#!/usr/bin/env bash
set -euo pipefail

readonly rng_test="$1"
output="$(mktemp)"

"$rng_test" --self_test | tee "$output"
grep -q "self-test passed" "${output}"
