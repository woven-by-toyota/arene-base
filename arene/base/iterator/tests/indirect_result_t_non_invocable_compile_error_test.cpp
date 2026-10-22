// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/indirect_result_t.hpp"

struct non_invocable_callable {};

// This should fail to compile and list a constraint violation on is_invocable_v
using result = arene::base::indirect_result_t<non_invocable_callable, int*>;
