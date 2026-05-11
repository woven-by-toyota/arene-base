// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file optional_test.cpp
/// @brief Unit tests for the public export header of the optional subpackage.
///
#include "arene/base/optional.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

namespace {
/// @test Ensure @c optional is exported
TEST(Optional, Optional) { using ::arene::base::optional; }
/// @test Ensure @c nullopt_t is exported
TEST(Optional, NullOptT) { using ::arene::base::nullopt_t; }
/// @test Ensure @c nullopt is exported
TEST(Optional, NullOpt) { using ::arene::base::nullopt; }
/// @test Ensure @c null_opt_t is exported
TEST(Optional, NullOptTOld) { using ::arene::base::null_opt_t; }
/// @test Ensure @c null_opt is exported
TEST(Optional, NullOptOld) { using ::arene::base::null_opt; }

/// @test Ensure @c bad_optional_access is exported
TEST(Optional, BadOptionalAccess) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
  using ::arene::base::bad_optional_access;
#else
  GTEST_SKIP() << "::arene::base::bad_optional_access is not available when exceptions are disabled.";
#endif
}

/// @test Ensure @c optional_resetter is exported
TEST(Optional, OptionalResetter) { using ::arene::base::optional_resetter; }
}  // namespace
