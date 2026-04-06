// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/detail/exceptions/are_exceptions_enabled.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test When exceptions are enabled by default on a platform, are_exceptions_enabled_v should be true.
TEST(AreExceptionsEnabled, ImplicitlyEnabledByConfigIsTrue) {
  STATIC_ASSERT_TRUE(arene::base::detail::are_exceptions_enabled_v);
}

}  // namespace
