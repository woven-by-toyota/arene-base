// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/iterator/detail/has_member_element_type.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::iterator_detail::has_member_element_type_v;

struct member_element_type {
  using element_type = int;
};

/// @test Given a type `T` for which the member type `element_type` is not defined, then
/// `has_member_element_type_v<T>` is `false`.
TEST(HasMemberElementTypeTest, IsFalseIfNoMemberElementType) {
  STATIC_ASSERT_FALSE(has_member_element_type_v<int&>);
  STATIC_ASSERT_FALSE(has_member_element_type_v<int const&>);
  STATIC_ASSERT_FALSE(has_member_element_type_v<int&&>);
  STATIC_ASSERT_FALSE(has_member_element_type_v<int const&&>);

  struct no_member_element {};
  STATIC_ASSERT_FALSE(has_member_element_type_v<no_member_element>);
  STATIC_ASSERT_FALSE(has_member_element_type_v<no_member_element const>);
  STATIC_ASSERT_FALSE(has_member_element_type_v<no_member_element&>);
  STATIC_ASSERT_FALSE(has_member_element_type_v<no_member_element const&>);
  STATIC_ASSERT_FALSE(has_member_element_type_v<no_member_element&&>);
  STATIC_ASSERT_FALSE(has_member_element_type_v<no_member_element const&&>);

  STATIC_ASSERT_FALSE(has_member_element_type_v<member_element_type&>);
  STATIC_ASSERT_FALSE(has_member_element_type_v<member_element_type const&>);
  STATIC_ASSERT_FALSE(has_member_element_type_v<member_element_type&&>);
  STATIC_ASSERT_FALSE(has_member_element_type_v<member_element_type const&&>);
}

/// @test Given a type `T` for which the member type `element_type` is defined, then
/// `has_member_element_type_v<T>` is `true`.
TEST(HasMemberElementTypeTest, IsTrueIfHasMemberElementType) {
  STATIC_ASSERT_TRUE(has_member_element_type_v<member_element_type>);
  STATIC_ASSERT_TRUE(has_member_element_type_v<member_element_type const>);
}

}  // namespace
