// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/functional/identity.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_transparent_comparator_for.hpp"

namespace {

using ::arene::base::identity;

using types = ::testing::Types<int&, int const&, int&&, int const&&, int*, int const*>;

template <typename T>
class IdentityPropertiesTest : public ::testing::Test {};

TYPED_TEST_SUITE(IdentityPropertiesTest, types, );

/// @test Given an instance of the identity function object, when invoked with a type with a given reference
/// qualification, then the return type is the same as the input type, as if forwarded via std::forward.
TYPED_TEST(IdentityPropertiesTest, ReturnTypeIsForwardedInputType) {
  ::testing::StaticAssertTypeEq<
      decltype(std::declval<identity>()(std::declval<TypeParam>())),
      decltype(std::forward<TypeParam>(std::declval<TypeParam>()))>();
}

/// @test Given an instance of the identity function object, then invoking it with any value is noexcept.
TYPED_TEST(IdentityPropertiesTest, IsNoexcept) {
  STATIC_ASSERT_TRUE(noexcept(std::declval<identity>()(std::declval<TypeParam>())));
}

/// @test The identity function object is a transparent comparator
TYPED_TEST(IdentityPropertiesTest, IsTransparent) {
  STATIC_ASSERT_TRUE(arene::base::is_transparent_comparator_v<identity>);
}

/// @test Given an instance of the identity function object, when it is invoked with a value, then it returns the value
/// unchanged.
TEST(IdentityValueTest, ReturnsInputUnchanged) {
  constexpr identity idn;
  STATIC_ASSERT_EQ(idn(42), 42);
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Testing for unmodified exact copy");
  STATIC_ASSERT_EQ(idn(3.14), 3.14);
  ARENE_IGNORE_END();
  constexpr auto str_val = "Hello";
  STATIC_ASSERT_STREQ(idn(str_val), str_val);
}

/// @test Given an instance of the identity function object, when it is invoked with a value, then it returns a
/// reference to the input value.
TEST(IdentityValueTest, ReturnsReferenceToInput) {
  identity const idn;
  int const int_val = 42;
  EXPECT_EQ(&idn(int_val), &int_val);
  double const double_val = 3.14;
  EXPECT_EQ(&idn(double_val), &double_val);
  char const* str_val = "Hello";
  EXPECT_EQ(idn(str_val), str_val);
}

}  // namespace
