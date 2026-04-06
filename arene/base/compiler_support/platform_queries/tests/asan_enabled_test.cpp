// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/platform_queries/asan_enabled.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries/has_feature.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Ensure `ARENE_ASAN_ENABLED` property check is defined
TEST(AreneAsanEnabled, IsProperlyDefinedConfig) {
  STATIC_ASSERT_TRUE(ARENE_GUARANTEE_INTERNAL_DEFINITION(ARENE_ASAN_ENABLED));
}

/// @test if Address Sanitizer is enabled, `ARENE_IS_ON(ARENE_ASAN_ENABLED)` is `true`, otherwise it is `false`.
TEST(AreneAsanEnabled, IsOnWhenEtherAddressSanitizerFeatureOrSanitizeAddressIsDefinedElseOff) {
#if defined(__SANITIZE_ADDRESS__)
  constexpr bool sanitize_address_defined = true;
#else
  constexpr bool sanitize_address_defined = false;
#endif
  if (ARENE_HAS_FEATURE(address_sanitizer) || sanitize_address_defined) {
    ASSERT_TRUE(ARENE_IS_ON(ARENE_ASAN_ENABLED));
  } else {
    ASSERT_TRUE(ARENE_IS_OFF(ARENE_ASAN_ENABLED));
  }
}

}  // namespace
