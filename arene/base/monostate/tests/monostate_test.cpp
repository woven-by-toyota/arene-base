// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/monostate/monostate.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_empty.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::monostate;
using arene::base::monostate_identity;

/// @test `monostate` is an empty class.
TEST(MonostateTest, IsEmpty) { STATIC_ASSERT_TRUE(std::is_empty<monostate>::value); }

/// @test `monostate` is default constructible.
TEST(MonostateTest, IsDefaultConstructible) {
  STATIC_ASSERT_TRUE(std::is_default_constructible<monostate>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_default_constructible<monostate>::value);
  STATIC_ASSERT_TRUE(std::is_nothrow_default_constructible<monostate>::value);
}

/// @test `monostate` is move constructible.
TEST(MonostateTest, IsMoveConstructible) {
  STATIC_ASSERT_TRUE(std::is_move_constructible<monostate>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_move_constructible<monostate>::value);
  STATIC_ASSERT_TRUE(std::is_nothrow_move_constructible<monostate>::value);
}

/// @test `monostate` is copy constructible.
TEST(MonostateTest, IsCopyConstructible) {
  STATIC_ASSERT_TRUE(std::is_copy_constructible<monostate>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_copy_constructible<monostate>::value);
  STATIC_ASSERT_TRUE(std::is_nothrow_copy_constructible<monostate>::value);
}

/// @test `monostate` is move assignable.
TEST(MonostateTest, IsMoveAssignable) {
  STATIC_ASSERT_TRUE(std::is_move_assignable<monostate>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_move_assignable<monostate>::value);
  STATIC_ASSERT_TRUE(std::is_nothrow_move_assignable<monostate>::value);
}

/// @test `monostate` is copy assignable.
TEST(MonostateTest, IsCopyAssignable) {
  STATIC_ASSERT_TRUE(std::is_copy_assignable<monostate>::value);
  STATIC_ASSERT_TRUE(std::is_trivially_copy_assignable<monostate>::value);
  STATIC_ASSERT_TRUE(std::is_nothrow_copy_assignable<monostate>::value);
}

/// @test `monostate_identity` turns `void` types to `monostate`.
TEST(MonostateTest, MonostateIdentity) {
  // void types
  ::testing::StaticAssertTypeEq<monostate_identity<void>, monostate>();
  ::testing::StaticAssertTypeEq<monostate_identity<void const>, monostate const>();
  ::testing::StaticAssertTypeEq<monostate_identity<void volatile>, monostate volatile>();
  ::testing::StaticAssertTypeEq<monostate_identity<void const volatile>, monostate const volatile>();

  // non-void types
  ::testing::StaticAssertTypeEq<monostate_identity<int>, int>();
  ::testing::StaticAssertTypeEq<monostate_identity<int const>, int const>();
  ::testing::StaticAssertTypeEq<monostate_identity<int volatile>, int volatile>();
  ::testing::StaticAssertTypeEq<monostate_identity<int const volatile>, int const volatile>();
}

/// @test `monostate` supports comparison operations.
TEST(MonostateTest, IsComparable) {
  constexpr monostate lhs;
  constexpr monostate rhs;

  // Equality/inequality
  STATIC_ASSERT_EQ(lhs, rhs);
  STATIC_ASSERT_FALSE(lhs != rhs);

  // Ordering
  STATIC_ASSERT_FALSE(lhs < rhs);
  STATIC_ASSERT_LE(lhs, rhs);
  STATIC_ASSERT_GE(lhs, rhs);
  STATIC_ASSERT_FALSE(lhs > rhs);

  // Noexcept-ness
  STATIC_ASSERT_TRUE(noexcept(lhs == rhs));
  STATIC_ASSERT_TRUE(noexcept(lhs != rhs));
  STATIC_ASSERT_TRUE(noexcept(lhs < rhs));
  STATIC_ASSERT_TRUE(noexcept(lhs <= rhs));
  STATIC_ASSERT_TRUE(noexcept(lhs >= rhs));
  STATIC_ASSERT_TRUE(noexcept(lhs > rhs));
}

/// @test `monostate` hashing provides consistent results.
TEST(MonostateTest, StdHash) {
  constexpr monostate first{};
  constexpr monostate second{};
  STATIC_ASSERT_EQ(std::hash<monostate>{}(first), std::hash<monostate>{}(second));
}

}  // namespace
