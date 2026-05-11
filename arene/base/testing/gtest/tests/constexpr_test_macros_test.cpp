// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @brief Provides tests that *should succeed* for CONSTEXPR_TEST and CONSTEXPR_TYPED_TEST
/// @note Tests that *should fail* cannot be tested in this file. Instead, those
/// are tested with separate Bazel targets.
///

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest/constexpr_test.hpp"  // IWYU pragma: keep

// IWYU pragma: no_include "arene/base/testing/gtest.hpp"

namespace {

/// @test check that a CONSTEXPR_TEST that should not fail, does not fail
CONSTEXPR_TEST(ConstexprTest, SuccessfulTest) {
  auto const arg1 = int{1};
  auto const arg2 = arg1;

  CONSTEXPR_ASSERT(2 == arg1 + arg2);
  CONSTEXPR_ASSERT_EQ(2, arg1 + arg2);
  CONSTEXPR_ASSERT_FALSE(3 == arg1 + arg2);
}

template <class T>
auto not_constexpr_fn(T value) {
  return value;
}

/// @test check that a CONDITIONALLY_CONSTEXPR_TEST that should not fail, does not fail
CONDITIONALLY_CONSTEXPR_TEST(ConstexprTest, ConditionallyConstexprSuccess, true) {
  auto const arg1 = int{1};
  auto const arg2 = arg1;

  CONSTEXPR_ASSERT(2 == arg1 + arg2);
  CONSTEXPR_ASSERT_EQ(2, arg1 + arg2);
  CONSTEXPR_ASSERT_FALSE(3 == arg1 + arg2);
}

/// @test check that a CONDITIONALLY_CONSTEXPR_TEST set to false can call a non-constexpr function
CONDITIONALLY_CONSTEXPR_TEST(ConstexprTest, NotConditionallyConstexprTest, false) {
  auto const arg = int{1};

  CONSTEXPR_ASSERT(arg == not_constexpr_fn(arg));
  CONSTEXPR_ASSERT_EQ(arg, not_constexpr_fn(arg));
}

template <class T>
struct ConstexprTypedTest : ::testing::Test {};

using test_types = ::testing::Types<int, unsigned>;

TYPED_TEST_SUITE(ConstexprTypedTest, test_types, );

/// @test check that a CONSTEXPR_TYPED_TEST that should not fail, does not fail
CONSTEXPR_TYPED_TEST(ConstexprTypedTest, SuccessfulTest) {
  auto const arg1 = TypeParam{1};
  auto const arg2 = arg1;

  CONSTEXPR_ASSERT(TypeParam{2} == arg1 + arg2);
  CONSTEXPR_ASSERT_EQ(TypeParam{2}, arg1 + arg2);
  CONSTEXPR_ASSERT_FALSE(TypeParam{3} == arg1 + arg2);
}

/// @test check that a CONDITIONALLY_CONSTEXPR_TYPED_TEST that should not fail, does not fail
CONDITIONALLY_CONSTEXPR_TYPED_TEST(ConstexprTypedTest, ConditionallyConstexprTest, true) {
  auto const arg1 = TypeParam{1};
  auto const arg2 = arg1;

  CONSTEXPR_ASSERT(TypeParam{2} == arg1 + arg2);
  CONSTEXPR_ASSERT_EQ(TypeParam{2}, arg1 + arg2);
  CONSTEXPR_ASSERT_FALSE(TypeParam{3} == arg1 + arg2);
}

constexpr auto constexpr_for_signed(int value) { return value; }

auto constexpr_for_signed(unsigned int value) { return value; }

/// @test check that a CONDITIONALLY_CONSTEXPR_TYPED_TEST can be used with a type trait
CONDITIONALLY_CONSTEXPR_TYPED_TEST(ConstexprTypedTest, WithTrait, (std::is_same<TypeParam, int>{})) {
  auto const arg = TypeParam{1};

  CONSTEXPR_ASSERT(arg == constexpr_for_signed(arg));
  CONSTEXPR_ASSERT_EQ(arg, constexpr_for_signed(arg));
}

/// @test check that a CONDITIONALLY_CONSTEXPR_TYPED_TEST set to false can call a non-constexpr function
CONDITIONALLY_CONSTEXPR_TYPED_TEST(ConstexprTypedTest, NotConditionallyConstexprTest, false) {
  auto const arg = TypeParam{1};

  CONSTEXPR_ASSERT(arg == not_constexpr_fn(arg));
  CONSTEXPR_ASSERT_EQ(arg, not_constexpr_fn(arg));
}

/// @test check that a CONDITIONAL_CONSTEXPR_TYPED_TEST that should not fail, does not fail
CONDITIONAL_CONSTEXPR_TYPED_TEST(ConstexprTypedTest, ConditionalSuccess, true) {
  auto const arg1 = TypeParam{1};
  auto const arg2 = arg1;

  CONSTEXPR_ASSERT(TypeParam{2} == arg1 + arg2);
  CONSTEXPR_ASSERT_EQ(TypeParam{2}, arg1 + arg2);
  CONSTEXPR_ASSERT_FALSE(TypeParam{3} == arg1 + arg2);
}

/// @test check that a CONDITIONAL_CONSTEXPR_TYPED_TEST that should not run is not compiled and does not run.
CONDITIONAL_CONSTEXPR_TYPED_TEST(ConstexprTypedTest, ConditionalDoesNotRun, false) {
  auto const arg = TypeParam{1};

  CONSTEXPR_ASSERT(arg == not_constexpr_fn(arg));
  CONSTEXPR_ASSERT_EQ(arg, not_constexpr_fn(arg));

  FAIL() << "This test should not run";
}

}  // namespace
