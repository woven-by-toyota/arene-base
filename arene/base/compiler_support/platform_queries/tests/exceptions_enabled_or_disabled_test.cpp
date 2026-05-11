// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries/exceptions_enabled.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

namespace {

#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
constexpr bool macro_enabled = true;
#else
constexpr bool macro_enabled = false;
#endif

/// @test @c ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED) should match whether they are enabled
TEST(AreneExceptionsEnabled, MatchesConfig) { ASSERT_EQ(macro_enabled, TEST_EXCEPTIONS_ENABLED); }

}  // namespace
