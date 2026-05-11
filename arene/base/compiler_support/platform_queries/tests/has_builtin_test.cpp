// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/platform_queries/has_builtin.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

ARENE_IGNORE_START();
ARENE_IGNORE_ARMCLANG("-Wunreachable-code", "Code is reachable for some compiler versions");

/// @test `ARENE_HAS_BUILTIN` yields `false` on QNX
TEST(HasBuiltin, NoBuiltinsOnQNX) {
#ifdef __QNX__
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_expect));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_trap));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_bswap64));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_bswap32));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_bswap16));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_fabs));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_fabsf));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_is_constant_evaluated));
#else
  GTEST_SKIP() << "Not on QNX";
#endif
}

/// @test On platforms other than QNX, `ARENE_HAS_BUILTIN` yields the same result as `__has_builtin` where
/// `__has_builtin` is available, and `false` otherwise
TEST(HasBuiltin, HasBuiltInReflectsPlatformNotOnQNX) {
#ifdef __QNX__
  GTEST_SKIP() << "On QNX";
#elif defined(__has_builtin)
  STATIC_ASSERT_EQ(ARENE_HAS_BUILTIN(__builtin_expect), __has_builtin(__builtin_expect));
  STATIC_ASSERT_EQ(ARENE_HAS_BUILTIN(__builtin_trap), __has_builtin(__builtin_trap));
  STATIC_ASSERT_EQ(ARENE_HAS_BUILTIN(__builtin_bswap64), __has_builtin(__builtin_bswap64));
  STATIC_ASSERT_EQ(ARENE_HAS_BUILTIN(__builtin_bswap32), __has_builtin(__builtin_bswap32));
  STATIC_ASSERT_EQ(ARENE_HAS_BUILTIN(__builtin_bswap16), __has_builtin(__builtin_bswap16));
  STATIC_ASSERT_EQ(ARENE_HAS_BUILTIN(__builtin_fabs), __has_builtin(__builtin_fabs));
  STATIC_ASSERT_EQ(ARENE_HAS_BUILTIN(__builtin_fabsf), __has_builtin(__builtin_fabsf));
  STATIC_ASSERT_EQ(ARENE_HAS_BUILTIN(__builtin_is_constant_evaluated), __has_builtin(__builtin_is_constant_evaluated));
  STATIC_ASSERT_EQ(ARENE_HAS_BUILTIN(__builtin_flibble), __has_builtin(__builtin_flibble));
#else
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_expect));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_trap));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_bswap64));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_bswap32));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_bswap16));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_fabs));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_fabsf));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_is_constant_evaluated));
  STATIC_ASSERT_FALSE(ARENE_HAS_BUILTIN(__builtin_flibble));
#endif
}

ARENE_IGNORE_END();

}  // namespace
