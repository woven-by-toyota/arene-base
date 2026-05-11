// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file static_assertions_config_disabled_test.cpp
/// @brief Test for when the config has been explicitly disabled.
///
#include <gtest/gtest.h>

#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest/static_assertions_config.hpp"

namespace {

/// @test Verify that @c ARENE_GTEST_STATIC_ASSERTIONS is a feature flag that can be on or off
TEST(AreneGtestStaticAssertions, IsValidConfig) {
  static_assert(
      ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_GTEST_STATIC_ASSERTIONS),
      "ARENE_GTEST_STATIC_ASSERTIONS should be a config still."
  );
}

/// @test Verify that @c ARENE_GTEST_STATIC_ASSERTIONS is off when explicitly disabled (which it should be for this
/// test)
TEST(AreneGtestStaticAssertions, IsOffIfExplicitlyDisabled) {
  static_assert(ARENE_IS_OFF(ARENE_GTEST_STATIC_ASSERTIONS), "Should be ARENE_IS_OFF if explicitly disabled");
}

}  // namespace
