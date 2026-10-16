// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/linalg/factor_result.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace {

using arene::base::linalg::factor_result;

/// @test Constructing a @c factor_result with the one unrepresentable pivot index is a precondition violation
TEST(FactorResultDeathTest, UnrepresentablePivotIndexIsPreconditionViolation) {
  std::size_t const unrepresentable_index{std::numeric_limits<std::size_t>::max()};

  ASSERT_DEATH(static_cast<void>(factor_result{unrepresentable_index}), "Precondition");
}

/// @test Obtaining the pivot index of a @c factor_result denoting success is a precondition violation
TEST(FactorResultDeathTest, PivotIndexWithoutPivotIndexIsPreconditionViolation) {
  factor_result const result{};

  ASSERT_DEATH(static_cast<void>(result.pivot_index()), "Precondition");
}

}  // namespace
