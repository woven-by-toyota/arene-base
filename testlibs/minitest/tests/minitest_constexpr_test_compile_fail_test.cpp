// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

auto non_constexpr_function() -> void {}

CONSTEXPR_TEST(Suite, FailsIfNotConstexprInvocable) { non_constexpr_function(); }
