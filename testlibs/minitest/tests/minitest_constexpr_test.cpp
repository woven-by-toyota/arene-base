// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/minitest.hpp"

namespace {

CONSTEXPR_TEST(Test, NonTypedTest) {
  ASSERT_TRUE(true);
  ASSERT_FALSE(false);
  ASSERT_EQ(1, 1);
}

CONSTEXPR_TEST(Test, NonTypedTestConstexprAssert) {
  CONSTEXPR_ASSERT(true);
  CONSTEXPR_ASSERT_FALSE(false);
  CONSTEXPR_ASSERT_EQ(1, 1);
}

constexpr bool false_constant = false;
constexpr bool true_constant = true;

auto runtime_only_function() -> bool { return true; }

CONDITIONALLY_CONSTEXPR_TEST(Test, NotReallyConstexpr, false_constant) {
  ASSERT_TRUE(runtime_only_function());
  ASSERT_FALSE(!runtime_only_function());
}

constexpr auto constant_function() -> bool { return true; }

CONDITIONALLY_CONSTEXPR_TEST(Test, ReallyConstexpr, true_constant && !false_constant) {
  ASSERT_TRUE(constant_function());
  ASSERT_FALSE(!constant_function());
}

using types_for_testing = ::testing::Types<char, int>;

template <typename T>
class TestSuiteForTypedTests : public testing::Test {};

TYPED_TEST_SUITE(TestSuiteForTypedTests, types_for_testing, );

CONSTEXPR_TYPED_TEST(TestSuiteForTypedTests, TypedTest) {
  ASSERT_TRUE(true);
  ASSERT_FALSE(false);
  ASSERT_EQ(0, TypeParam{});
}

template <typename T>
struct constexpr_test_check {
  static constexpr bool run_in_constexpr = false;
};

template <>
struct constexpr_test_check<int> {
  static constexpr bool run_in_constexpr = true;
};

constexpr auto maybe_runtime_only_function(int) -> bool { return true; }
auto maybe_runtime_only_function(char) -> bool { return true; }

// Note: If the test macro runs across multiplie lines, then gcc and clang report different line numbers for the macro.
// This makes writing the minitest_output_tests difficult, as we need the exact output, so keep these definitions on a
// single line.

// clang-format off
CONDITIONALLY_CONSTEXPR_TYPED_TEST(TestSuiteForTypedTests,ConditionalTypedTest,constexpr_test_check<TypeParam>::run_in_constexpr) {
  // clang-format on
  ASSERT_TRUE(maybe_runtime_only_function(TypeParam{}));
  ASSERT_FALSE(false);
  ASSERT_EQ(0, TypeParam{});
}

// clang-format off
CONDITIONAL_CONSTEXPR_TYPED_TEST(TestSuiteForTypedTests,ConditionalTypedTestRuns,constexpr_test_check<TypeParam>::run_in_constexpr) {
  // clang-format on
  ASSERT_TRUE(maybe_runtime_only_function(TypeParam{}));

  if (!constexpr_test_check<TypeParam>::run_in_constexpr) {
    FAIL() << "This test should not run if the condition is not met";
  }
}

}  // namespace
