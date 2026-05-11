// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

using types_for_testing = ::testing::Types<int>;

template <typename T>
class Suite : public testing::Test {};

TYPED_TEST_SUITE(Suite, types_for_testing, );

auto non_constexpr_function() -> void {}

#ifdef MINITEST_RUN_IN_CONSTEXPR
constexpr bool run_in_constexpr = true;
#else
constexpr bool run_in_constexpr = false;
#endif

CONDITIONALLY_CONSTEXPR_TYPED_TEST(Suite, FailsIfNotConstexprInvocable, run_in_constexpr) { non_constexpr_function(); }
