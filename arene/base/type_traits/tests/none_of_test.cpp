// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file none_of_test.cpp
/// @brief Unit tests for none_of.hpp
///
///

#include "arene/base/type_traits/none_of.hpp"

#include <gtest/gtest.h>

namespace {

using ::arene::base::none_of_v;

/// @test `none_of_v` evaluates to `true` for an empty parameter list
TEST(NoneOfTest, IsTrueForEmptyParameterPack) {
  static_assert(none_of_v<>, "Expected empty parameter pack to be false.");
  ASSERT_TRUE(none_of_v<>);
}

/// @test `none_of_v` evaluates to `false` if all values in the parameter list are `true`
TEST(NoneOfTest, IsFalseForParameterPackWithAllTrueValues) {
  static_assert(!none_of_v<true>, "Expected parameter pack to be false.");
  ASSERT_FALSE(none_of_v<true>);

  static_assert(!none_of_v<true, true>, "Expected parameter pack to be false.");
  ASSERT_FALSE((none_of_v<true, true>));

  static_assert(!none_of_v<true, true, true>, "Expected parameter pack to be false.");
  ASSERT_FALSE((none_of_v<true, true, true>));
}

/// @test `none_of_v` evaluates to `false` if any of the values in the parameter list are `true`
TEST(NoneOfTest, IsFalseForParameterPackWithAnyTrueValues) {
  static_assert(!none_of_v<true, false>, "Expected parameter pack to be false.");
  ASSERT_FALSE((none_of_v<true, false>));

  static_assert(!none_of_v<false, true, false>, "Expected parameter pack to be false.");
  ASSERT_FALSE((none_of_v<false, true, false>));

  static_assert(!none_of_v<false, false, true>, "Expected parameter pack to be false.");
  ASSERT_FALSE((none_of_v<false, false, true>));
}

/// @test `none_of_v` evaluates to `true` if all values in the parameter list are `false`
TEST(NoneOfTest, IsTrueForParameterPackWithAllFalseValues) {
  static_assert(none_of_v<false>, "Expected parameter pack to be true.");
  ASSERT_TRUE(none_of_v<false>);

  static_assert(none_of_v<false, false>, "Expected parameter pack to be true.");
  ASSERT_TRUE((none_of_v<false, false>));

  static_assert(none_of_v<false, false, false>, "Expected parameter pack to be true.");
  ASSERT_TRUE((none_of_v<false, false, false>));
}

}  // namespace
