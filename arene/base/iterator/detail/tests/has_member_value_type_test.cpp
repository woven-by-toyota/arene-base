// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/detail/has_member_value_type.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::iterator_detail::has_member_value_type_v;

struct member_value_type {
  using value_type = int;
};

/// @test Given a type `T` for which the member type `value_type` is not defined, then
/// `has_member_value_type_v<T>` is `false`.
TEST(HasMemberValueTypeTest, IsFalseIfNoMemberValueType) {
  STATIC_ASSERT_FALSE(has_member_value_type_v<int&>);
  STATIC_ASSERT_FALSE(has_member_value_type_v<int const&>);
  STATIC_ASSERT_FALSE(has_member_value_type_v<int&&>);
  STATIC_ASSERT_FALSE(has_member_value_type_v<int const&&>);

  struct no_member_value {};
  STATIC_ASSERT_FALSE(has_member_value_type_v<no_member_value>);
  STATIC_ASSERT_FALSE(has_member_value_type_v<no_member_value const>);
  STATIC_ASSERT_FALSE(has_member_value_type_v<no_member_value&>);
  STATIC_ASSERT_FALSE(has_member_value_type_v<no_member_value const&>);
  STATIC_ASSERT_FALSE(has_member_value_type_v<no_member_value&&>);
  STATIC_ASSERT_FALSE(has_member_value_type_v<no_member_value const&&>);

  STATIC_ASSERT_FALSE(has_member_value_type_v<member_value_type&>);
  STATIC_ASSERT_FALSE(has_member_value_type_v<member_value_type const&>);
  STATIC_ASSERT_FALSE(has_member_value_type_v<member_value_type&&>);
  STATIC_ASSERT_FALSE(has_member_value_type_v<member_value_type const&&>);
}

/// @test Given a type `T` for which the member type `value_type` is defined, then
/// `has_member_value_type_v<T>` is `true`.
TEST(HasMemberValueTypeTest, IsTrueIfHasMemberValueType) {
  STATIC_ASSERT_TRUE(has_member_value_type_v<member_value_type>);
  STATIC_ASSERT_TRUE(has_member_value_type_v<member_value_type const>);
}

}  // namespace
