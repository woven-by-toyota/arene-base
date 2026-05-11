// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/platform_queries/has_builtin_is_constant_evaluated.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries/compiler_info.hpp"
#include "arene/base/compiler_support/platform_queries/has_builtin.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Ensure `ARENE_HAS_BUILTIN_IS_CONSTANT_EVALUATED` property check is defined
TEST(AreneHasBuiltinIsConstantEvaluated, IsProperlyDefinedConfig) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_HAS_BUILTIN_IS_CONSTANT_EVALUATED));
}

constexpr bool has_builtin = ARENE_HAS_BUILTIN(__builtin_is_constant_evaluated);

/// @test `ARENE_IS_ON(ARENE_HAS_BUILTIN_IS_CONSTANT_EVALUATED)` evaluates to `true` if the built-in is available
TEST(AreneHasBuiltinIsConstantEvaluated, IsOnIfBuiltinIsDefined) {
  if (!has_builtin) {
    GTEST_SKIP() << "Only relevant on platform with builtin defined";
  }
  EXPECT_TRUE(ARENE_IS_ON(ARENE_HAS_BUILTIN_IS_CONSTANT_EVALUATED));
}

constexpr bool is_gcc_where_defined = ARENE_IS_ON(ARENE_COMPILER_GCC) && __GNUC__ >= 9;

/// @test `ARENE_IS_ON(ARENE_HAS_BUILTIN_IS_CONSTANT_EVALUATED)` evaluates to `true` if the compiler is gcc 9 or later
TEST(AreneHasBuiltinIsConstantEvaluated, IsOnForGccGreaterThan9) {
  if (!is_gcc_where_defined) {
    GTEST_SKIP() << "Only relevant on GCC >= 9";
  }
  EXPECT_TRUE(ARENE_IS_ON(ARENE_HAS_BUILTIN_IS_CONSTANT_EVALUATED));
}

/// @test `ARENE_IS_OFF(ARENE_HAS_BUILTIN_IS_CONSTANT_EVALUATED)` evaluates to `true` if the built-in is not available,
/// and the compiler is not gcc 9 or later
TEST(AreneHasBuiltinIsConstantEvaluated, IsOffIfBuiltinIsNotDefined) {
  if (has_builtin || is_gcc_where_defined) {
    GTEST_SKIP() << "Only relevant on platforms without the builtin";
  }
  EXPECT_TRUE(ARENE_IS_OFF(ARENE_HAS_BUILTIN_IS_CONSTANT_EVALUATED));
}

/// @test `ARENE_IS_OFF(ARENE_HAS_BUILTIN_IS_CONSTANT_EVALUATED)` evaluates to `true` on QNX
TEST(AreneHasBuiltinIsConstantEvaluated, IsOffOnQNX) {
#ifdef __QNX__
  STATIC_ASSERT_FALSE(has_builtin);
  STATIC_ASSERT_TRUE(ARENE_IS_OFF(ARENE_HAS_BUILTIN_IS_CONSTANT_EVALUATED));
#else
  GTEST_SKIP() << "Not on QNX";
#endif
}

}  // namespace
