// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file all_of_test.cpp
/// @brief Unit tests for all_of.hpp
///
///

#include "arene/base/type_traits/all_of.hpp"

#include <gtest/gtest.h>

namespace {

using ::arene::base::all_of_v;

/// @test `all_of_v` evaluates to `true` if evaluated with no parameters
TEST(AllOfVTest, IsTrueForEmptyParameterPack) {
  static_assert(all_of_v<>, "Expected empty parameter pack to be true.");
  ASSERT_TRUE(all_of_v<>);
}

/// @test `all_of_v` evaluates to `true` if all parameters are `true`
TEST(AllOfVTest, IsTrueForParameterPackWithAllTrueValues) {
  static_assert(all_of_v<true>, "Expected parameter pack to be true.");
  ASSERT_TRUE(all_of_v<true>);

  static_assert(all_of_v<true, true>, "Expected parameter pack to be true.");
  ASSERT_TRUE((all_of_v<true, true>));

  static_assert(all_of_v<true, true, true>, "Expected parameter pack to be true.");
  ASSERT_TRUE((all_of_v<true, true, true>));
}

/// @test `all_of_v` evaluates to `false` if any of the  parameters are `false`
TEST(AllOfVTest, IsFalseForParameterPackWithAnyFalseValues) {
  static_assert(!all_of_v<false, true>, "Expected parameter pack to be false.");
  ASSERT_FALSE((all_of_v<false, true>));

  static_assert(!all_of_v<true, false>, "Expected parameter pack to be false.");
  ASSERT_FALSE((all_of_v<false, false>));
}

/// @test `all_of_v` evaluates to `false` if all of the  parameters are `false`
TEST(NoneOfTest, IsFalseForParameterPackWithAllFalseValues) {
  static_assert(!all_of_v<false>, "Expected parameter pack to be false.");
  ASSERT_FALSE(all_of_v<false>);

  static_assert(!all_of_v<false, false>, "Expected parameter pack to be false.");
  ASSERT_FALSE((all_of_v<false, false>));
}

}  // namespace
