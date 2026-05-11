// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file any_of_test.cpp
/// @brief Unit tests for any_of.hpp
///
///

#include "arene/base/type_traits/any_of.hpp"

#include <gtest/gtest.h>

namespace {

using ::arene::base::any_of_v;

/// @test `any_of_v` evaluates to `false` if instantiated with no parameters
TEST(AnyOfVTest, IsFalseForEmptyParameterPack) {
  static_assert(!any_of_v<>, "Expected empty parameter pack to be false.");
  ASSERT_FALSE(any_of_v<>);
}

/// @test `any_of_v` evaluates to `true` if instantiated with all `true` parameters
TEST(AnyOfVTest, IsTrueForParameterPackWithAllTrueValues) {
  static_assert(any_of_v<true>, "Expected parameter pack to be true.");
  ASSERT_TRUE(any_of_v<true>);

  static_assert(any_of_v<true, true>, "Expected parameter pack to be true.");
  ASSERT_TRUE((any_of_v<true, true>));

  static_assert(any_of_v<true, true, true>, "Expected parameter pack to be true.");
  ASSERT_TRUE((any_of_v<true, true, true>));
}

/// @test `any_of_v` evaluates to `true` if any of the template parameters are `true`
TEST(AnyOfVTest, IsTrueForParameterPackWithAnyTrueValues) {
  static_assert(any_of_v<true, false>, "Expected parameter pack to be true.");
  ASSERT_TRUE((any_of_v<true, false>));

  static_assert(any_of_v<false, true, false>, "Expected parameter pack to be true.");
  ASSERT_TRUE((any_of_v<false, true, false>));

  static_assert(any_of_v<false, false, true>, "Expected parameter pack to be true.");
  ASSERT_TRUE((any_of_v<false, false, true>));
}

/// @test `any_of_v` evaluates to `false` if all of the template parameters are `false`
TEST(AnyOfVTest, IsFalseForParameterPackWithAllFalseValues) {
  static_assert(!any_of_v<false>, "Expected parameter pack to be false.");
  ASSERT_FALSE(any_of_v<false>);

  static_assert(!any_of_v<false, false>, "Expected parameter pack to be false.");
  ASSERT_FALSE((any_of_v<false, false>));
}

}  // namespace
