#!/usr/bin/env bash

# Copyright 2026, Toyota Motor Corporation
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set -euxo pipefail

log="$1"
length="$2"

expected="arene/base/contracts/tests/contract_output.cpp:11: error: Precondition violation: false"

[[ "$(head -n1 "$log")" == "$expected" ]]
eval "(( $(wc -l <"$log") $length ))"
