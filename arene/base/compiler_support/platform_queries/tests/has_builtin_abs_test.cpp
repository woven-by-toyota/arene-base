// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/platform_queries/has_builtin_abs.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Ensure `ARENE_HAS_BUILTIN_FABS` property check is defined
TEST(BuiltinAbs, HasBuiltinFabsIsProperlyDefinedAsConfig) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_HAS_BUILTIN_FABS));
}

/// @test Ensure `ARENE_HAS_BUILTIN_FABSF` property check is defined
TEST(BuiltinAbs, HasBuiltinFabsfIsProperlyDefinedAsConfig) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_HAS_BUILTIN_FABSF));
}

/// @test No built-in `fabs` on QNX
TEST(BuiltinAbs, BuiltinFabsNotAvailableOnQNX) {
#ifdef __QNX__
  STATIC_ASSERT_FALSE(ARENE_IS_ON(ARENE_HAS_BUILTIN_FABS));
#else
  GTEST_SKIP() << "Not on QNX";
#endif
}

/// @test No built-in `fabsf` on QNX
TEST(BuiltinAbs, BuiltinFabsfNotAvailableOnQNX) {
#ifdef __QNX__
  STATIC_ASSERT_FALSE(ARENE_IS_ON(ARENE_HAS_BUILTIN_FABSF));
#else
  GTEST_SKIP() << "Not on QNX";
#endif
}

}  // namespace
