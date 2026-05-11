// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/is_possibly_constant_evaluated.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

namespace {
/// @test `is_possibly_constant_evaluated` should yield `false` in a normal function if the compiler supports the check,
/// otherwise it returns `true`
TEST(IsPossiblyConstantEvaluated, PlainFunctionNotConstantEvaluated) {
#if ARENE_IS_ON(ARENE_COMPILER_GCC) && (__GNUC__ < 9)
  // gcc 8 cannot tell, so always "possibly" constant evaluated
  ASSERT_TRUE(arene::base::is_possibly_constant_evaluated());
#else
  ASSERT_FALSE(arene::base::is_possibly_constant_evaluated());
#endif
}

constexpr auto constexpr_check(int) -> bool { return arene::base::is_possibly_constant_evaluated(); }

/// @test `is_possibly_constant_evaluated` returns `true` when invoked from a `constexpr` function evaluated during
/// constant evaluation
TEST(IsPossiblyConstantEvaluated, CheckInConstantExpression) {
  static_assert(constexpr_check(42), "Must be true in constant expression");
}

/// @test `is_possibly_constant_evaluated` returns `false` when invoked from a `constexpr` function evaluated during
/// runtime, if the compiler supports the check
TEST(IsPossiblyConstantEvaluated, ConstexprFunctionAtRuntime) {
  ARENE_IGNORE_START();
  ARENE_IGNORE_ARMCLANG("-Wunreachable-code", "Code is reachable for some compiler versions");
  if (ARENE_IS_ON(ARENE_COMPILER_GCC) && (__GNUC__ <= 8)) {
    GTEST_SKIP() << "Will never return false as needed intrinsics are not supported by the current compiler";
  }
  ARENE_IGNORE_END();
  int value = 0;  // NOLINT(misc-const-correctness) explicitly testing non-const evaluable
  ASSERT_FALSE(constexpr_check(value));
}
}  // namespace
