// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/testing/gtest/detail/static_assertions_helpers.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest/detail/static_assertions_enabled.hpp"

namespace {

/// @test `c_strings_are_equal` compares C strings in a `constexpr`-compatible fashion
TEST(StaticAssertCStringsAreEqual, ComparesCStrings) {
  // This test is needed because coverage credit isn't given through the macros for this executable line.
  STATIC_ASSERT_TRUE(arene::base::testing::gtest::detail::c_strings_are_equal("", ""));
  STATIC_ASSERT_FALSE(arene::base::testing::gtest::detail::c_strings_are_equal("", "a"));
  STATIC_ASSERT_FALSE(arene::base::testing::gtest::detail::c_strings_are_equal("a", ""));
  STATIC_ASSERT_TRUE(arene::base::testing::gtest::detail::c_strings_are_equal("abc", "abc"));
  STATIC_ASSERT_FALSE(arene::base::testing::gtest::detail::c_strings_are_equal("a", "abc"));
  STATIC_ASSERT_FALSE(arene::base::testing::gtest::detail::c_strings_are_equal("abc", "a"));
}

}  // namespace
