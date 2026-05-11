// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief A simple user-defined type
class some_user_type {};

/// @brief A class that inherits from another class
class child_class : public some_user_type {};

using non_class_types = arene::base::type_lists::concat_unique_t<
    ::testing::array_types,
    ::testing::enum_types,
    ::testing::function_types,
    ::testing::member_pointer_types,
    ::testing::pointer_types,
    ::testing::reference_types,
    ::testing::scalar_types,
    ::testing::union_types>;
using class_types = arene::base::type_lists::concat_unique_t<::testing::class_types, ::testing::Types<child_class>>;

/// @brief Assert @c is_class<Input> derives from @c std::false_type and @c is_class_v<Type> is @c false
/// @tparam Input The input type to check if class type
template <typename Input>
constexpr auto static_assert_is_class_false() noexcept -> void {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_class<Input>, std::false_type>);
  ASSERT_FALSE(std::is_class_v<Input>);
}

/// @brief Assert @c is_class<Input> derives from @c std::true_type and @c is_class_v<Type> is @c true
/// @tparam Input The input type to check if class type
template <typename Input>
constexpr auto static_assert_is_class_true() noexcept -> void {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_class<Input>, std::true_type>);
  ASSERT_TRUE(std::is_class_v<Input>);
}

template <typename T>
class IsNonClassTest : public testing::Test {};

TYPED_TEST_SUITE(IsNonClassTest, non_class_types, );

/// @test @c is_class is derived from @c false_type and @c is_class_v is @c false for types that are not @c class types
CONSTEXPR_TYPED_TEST(IsNonClassTest, WithNonClassTypes) {
  static_assert_is_class_false<TypeParam>();
  static_assert_is_class_false<TypeParam const>();
  static_assert_is_class_false<TypeParam volatile>();
  static_assert_is_class_false<TypeParam const volatile>();
  static_assert_is_class_false<TypeParam&>();
  static_assert_is_class_false<TypeParam const&>();
  static_assert_is_class_false<TypeParam volatile&>();
  static_assert_is_class_false<TypeParam const volatile&>();
  static_assert_is_class_false<TypeParam&&>();
  static_assert_is_class_false<TypeParam const&&>();
  static_assert_is_class_false<TypeParam volatile&&>();
  static_assert_is_class_false<TypeParam const volatile&&>();
}

template <typename T>
class IsClassTest : public testing::Test {};

TYPED_TEST_SUITE(IsClassTest, class_types, );

/// @test @c is_class is derived from @c true_type and @c is_class_v is @c true for @c class types
CONSTEXPR_TYPED_TEST(IsClassTest, WithClassTypes) {
  static_assert_is_class_true<TypeParam>();
  static_assert_is_class_true<TypeParam const>();
  static_assert_is_class_true<TypeParam volatile>();
  static_assert_is_class_true<TypeParam const volatile>();
}

/// @test @c is_class is derived from @c false_type and @c is_class_v is @c false for @c class reference types
CONSTEXPR_TYPED_TEST(IsClassTest, WithClassReferenceTypes) {
  static_assert_is_class_false<TypeParam&>();
  static_assert_is_class_false<TypeParam const&>();
  static_assert_is_class_false<TypeParam volatile&>();
  static_assert_is_class_false<TypeParam const volatile&>();
  static_assert_is_class_false<TypeParam&&>();
  static_assert_is_class_false<TypeParam const&&>();
  static_assert_is_class_false<TypeParam volatile&&>();
  static_assert_is_class_false<TypeParam const volatile&&>();
}

}  // namespace
