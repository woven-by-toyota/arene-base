// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_arithmetic.hpp"

// Explcitly testing a particular file, we don't use the external header
// IWYU pragma: no_include "arene/base/testing/gtest.hpp"
#include "arene/base/testing/gtest/conditional_tests.hpp"  // IWYU pragma: keep

namespace {

template <class T>
class ConditionalTestMacrosFixture : public testing::Test {
 protected:
  using type_alias = std::true_type;

  bool true_value_{};
  bool false_value_{};

  void SetUp() override {
    true_value_ = true;
    false_value_ = false;
  }
};

using types = ::testing::Types<int, double>;

TYPED_TEST_SUITE(ConditionalTestMacrosFixture, types, );

/// @test Test that conditional test can access test fixture.
CONDITIONAL_TYPED_TEST(ConditionalTestMacrosFixture, ShouldRun, std::is_arithmetic<TypeParam>::value) {
  static_assert(TestFixture::type_alias::value, "Conditional test should be able to access fixture type");

  ASSERT_TRUE(this->true_value_);
  ASSERT_FALSE(this->false_value_);
}

ARENE_IGNORE_START();
ARENE_IGNORE_ARMCLANG("-Wunused-const-variable", "false positive");
template <bool B, class... Args>
constexpr auto dependent_bool_v = B;
ARENE_IGNORE_END();

/// @test Test that conditional test is not run when the condition is false.
CONDITIONAL_TYPED_TEST(ConditionalTestMacrosFixture, ShouldNotRun, !std::is_arithmetic<TypeParam>::value) {
  static_assert(dependent_bool_v<false, TypeParam>, "This should not be compiled due to the conditional");
  ASSERT_TRUE(false);
}

}  // namespace
