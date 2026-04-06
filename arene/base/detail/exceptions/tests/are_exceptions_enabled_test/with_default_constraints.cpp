// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/detail/exceptions/are_exceptions_enabled.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test When `ARENE_EXCEPTIONS_ENABLED` is defined, `are_exceptions_enabled_v` should be true.
TEST(AreExceptionsEnabled, IsEnabledIfAreneExceptionsEnabledIsDefined) {
#if ARENE_IS_ON(ARENE_EXCEPTIONS_ENABLED)
  constexpr bool expected = true;
#else
  constexpr bool expected = false;
#endif
  STATIC_ASSERT_EQ(arene::base::detail::are_exceptions_enabled_v, expected);
}

}  // namespace
