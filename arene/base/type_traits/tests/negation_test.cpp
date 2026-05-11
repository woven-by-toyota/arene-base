// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/negation.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::negation;
using ::arene::base::negation_v;

/// @test Given a boolean trait `BoolTrait`, then the value of `negation<BoolTrait>::value` is `!BoolTrait::value`
TEST(Negation, SingleParameterIsNegatedValue) {
  STATIC_ASSERT_FALSE(negation<std::true_type>::value);
  STATIC_ASSERT_TRUE(negation<std::false_type>::value);
}

/// @test Given a boolean trait `BoolTraits`, then the variable template `negation_v<BoolTraits...>` is
/// equivalent to `negation<BoolTrait>::value`
TEST(NegationV, IsEquivalentToValueOfNegation) {
  STATIC_ASSERT_EQ(negation<std::true_type>::value, negation_v<std::true_type>);
  STATIC_ASSERT_EQ(negation<std::false_type>::value, negation_v<std::false_type>);
}

}  // namespace
