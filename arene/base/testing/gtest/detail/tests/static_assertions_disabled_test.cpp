// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file static_assertions_test.cpp
/// @brief provides unit tests for static_assertions_disabled.hpp
///

#include "arene/base/testing/gtest/detail/static_assertions_disabled.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/integral_constant.hpp"

namespace {

constexpr auto add_one_constexpr(int value) -> int { return value + 1; }

TEST(StaticAssertions, StaticAssertEq) {
  STATIC_ASSERT_EQ(10, 10);
  STATIC_ASSERT_EQ(-10, -10);
  STATIC_ASSERT_EQ(add_one_constexpr(10), 11);
  STATIC_ASSERT_EQ(10 + 11, 21);
}

TEST(StaticAssertions, StaticAssertNe) {
  STATIC_ASSERT_NE(10, 11);
  STATIC_ASSERT_NE(10, -11);
  STATIC_ASSERT_NE(-10, -11);
  STATIC_ASSERT_NE(-10, 11);
  STATIC_ASSERT_NE(add_one_constexpr(10), 10);
  STATIC_ASSERT_NE(10 + 11, 20);
}

TEST(StaticAssertions, StaticAssertLt) {
  STATIC_ASSERT_LT(10, 11);
  STATIC_ASSERT_LT(-10, 11);
  STATIC_ASSERT_LT(-11, -10);
  STATIC_ASSERT_LT(add_one_constexpr(10), 12);
  STATIC_ASSERT_LT(10 + 11, 22);
}

TEST(StaticAssertions, StaticAssertLe) {
  STATIC_ASSERT_LE(10, 11);
  STATIC_ASSERT_LE(-10, 11);
  STATIC_ASSERT_LE(-11, -10);
  STATIC_ASSERT_LE(add_one_constexpr(10), 12);
  STATIC_ASSERT_LE(10 + 11, 22);

  STATIC_ASSERT_LE(10, 10);
  STATIC_ASSERT_LE(add_one_constexpr(10), 11);
  STATIC_ASSERT_LE(10 + 11, 21);
}

TEST(StaticAssertions, StaticAssertGt) {
  STATIC_ASSERT_GT(11, 10);
  STATIC_ASSERT_GT(11, -10);
  STATIC_ASSERT_GT(-10, -11);
  STATIC_ASSERT_GT(add_one_constexpr(10), 10);
  STATIC_ASSERT_GT(10 + 11, 02);
}

TEST(StaticAssertions, StaticAssertGe) {
  STATIC_ASSERT_GE(11, 10);
  STATIC_ASSERT_GE(11, -10);
  STATIC_ASSERT_GE(-10, -11);
  STATIC_ASSERT_GE(add_one_constexpr(10), 10);
  STATIC_ASSERT_GE(10 + 11, 02);

  STATIC_ASSERT_GE(10, 10);
  STATIC_ASSERT_GE(add_one_constexpr(10), 11);
  STATIC_ASSERT_GE(10 + 11, 21);
}

template <typename T>
struct some_trait : std::false_type {};

template <>
struct some_trait<int> : std::true_type {};

template <typename U, typename V>
struct multi_arg_trait : std::false_type {};

template <typename V>
struct multi_arg_trait<int, V> : std::true_type {};

template <bool Value>
struct implicitly {
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr operator bool() const { return Value; }
};

TEST(StaticAssertions, StaticAssertTrue) {
  STATIC_ASSERT_TRUE(true);
  STATIC_ASSERT_TRUE(!false);
  STATIC_ASSERT_TRUE(1 + 2 + 3 == 6);
  STATIC_ASSERT_TRUE(add_one_constexpr(10) == 11);
  STATIC_ASSERT_TRUE(some_trait<int>::value);
  STATIC_ASSERT_TRUE(!some_trait<float>::value);
  STATIC_ASSERT_TRUE(multi_arg_trait<int, float>::value);
  STATIC_ASSERT_TRUE(implicitly<true>{});
}

TEST(StaticAssertions, StaticAssertFalse) {
  STATIC_ASSERT_FALSE(!true);
  STATIC_ASSERT_FALSE(false);
  STATIC_ASSERT_FALSE(1 + 2 + 3 != 6);
  STATIC_ASSERT_FALSE(add_one_constexpr(10) != 11);
  STATIC_ASSERT_FALSE(some_trait<float>::value);
  STATIC_ASSERT_FALSE(!some_trait<int>::value);
  STATIC_ASSERT_FALSE(!multi_arg_trait<int, float>::value);
  STATIC_ASSERT_FALSE(implicitly<false>{});
}

TEST(StaticAssertions, StaticAssertNear) {
  STATIC_ASSERT_NEAR(0.0, 0.0, 1.0);
  STATIC_ASSERT_NEAR(1.0, 0.0, 1.0000000001);
  STATIC_ASSERT_NEAR(1.0, 1.05, 0.1);
  STATIC_ASSERT_NEAR(1.05, 1.00, 0.1);
  STATIC_ASSERT_NEAR(1.05, 1.05, 0.1);
  STATIC_ASSERT_NEAR(0.0, 0.0, 0.0);
  STATIC_ASSERT_NEAR(-1.05, -1.00, 0.1);
}

TEST(StaticAssertions, StaticAssertStrEq) {
  STATIC_ASSERT_STREQ("a", "a");
  STATIC_ASSERT_STREQ("whole phrase", "whole phrase");
  constexpr auto lhs = "some literal";
  constexpr auto rhs = "some literal";
  STATIC_ASSERT_STREQ(lhs, rhs);
  STATIC_ASSERT_STREQ(lhs, lhs);
}

TEST(StaticAssertions, StaticAssertStrNe) {
  STATIC_ASSERT_STRNE("a", "b");
  STATIC_ASSERT_STRNE("whole phrase", "different phrase");
  constexpr auto lhs = "some literal";
  constexpr auto rhs = "some other literal";
  STATIC_ASSERT_STRNE(lhs, rhs);
}

}  // namespace
