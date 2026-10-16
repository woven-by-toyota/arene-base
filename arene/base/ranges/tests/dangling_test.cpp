// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/ranges/dangling.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"

namespace {

using ::arene::base::ranges::dangling;

/// @test `arene::base::dangling` is nothrow default constructible
TEST(Dangling, IsNothrowDefaultConstructible) {
  STATIC_ASSERT_TRUE(std::is_nothrow_default_constructible<dangling>::value);
}

/// @test `arene::base::dangling` is nothrow copy constructible
TEST(Dangling, IsNothrowCopyConstructible) {
  STATIC_ASSERT_TRUE(std::is_nothrow_constructible<dangling, dangling const&>::value);
}
/// @test `arene::base::dangling` is nothrow move constructible
TEST(Dangling, IsNothrowMoveConstructible) {
  STATIC_ASSERT_TRUE(std::is_nothrow_constructible<dangling, dangling&&>::value);
}
/// @test `arene::base::dangling` is nothrow copy assignable
TEST(Dangling, IsNothrowCopyAssignable) {
  STATIC_ASSERT_TRUE(std::is_nothrow_assignable<dangling, dangling const&>::value);
}
/// @test `arene::base::dangling` is nothrow move assignable
TEST(Dangling, IsNothrowMoveAssignable) { STATIC_ASSERT_TRUE(std::is_nothrow_assignable<dangling, dangling&&>::value); }

/// @test `arene::base::dangling` is constructible from any arbitrary combination of types
TEST(Dangling, IsConstructableFromAnything) {
  STATIC_ASSERT_TRUE(std::is_constructible<dangling, int&>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<dangling, int&&>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<dangling, int const&>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<dangling, int const&&>::value);

  STATIC_ASSERT_TRUE(std::is_constructible<dangling, int*>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<dangling, int*>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<dangling, int* const>::value);
  STATIC_ASSERT_TRUE(std::is_constructible<dangling, int const* const>::value);

  STATIC_ASSERT_TRUE(std::is_constructible<dangling, int*, double, float>::value);

  STATIC_ASSERT_TRUE(std::is_constructible<dangling, dangling, int>::value);
}

/// @test `arene::base::dangling` is not equality comparable
TEST(Dangling, IsNotEqualityComparable) { STATIC_ASSERT_TRUE(!arene::base::is_equality_comparable_v<dangling>); }

/// @test `arene::base::dangling` is not inequality comparable
TEST(Dangling, IsNotInequalityComparable) { STATIC_ASSERT_TRUE(!arene::base::is_inequality_comparable_v<dangling>); }

}  // namespace
