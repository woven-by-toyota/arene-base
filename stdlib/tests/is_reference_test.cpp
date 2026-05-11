// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

using non_reference_types = arene::base::type_lists::concat_unique_t<
    ::testing::array_types,
    ::testing::class_types,
    ::testing::enum_types,
    ::testing::function_types,
    ::testing::member_pointer_types,
    ::testing::pointer_types,
    ::testing::scalar_types,
    ::testing::union_types>;

/// @brief Assert @c is_reference<Input> derives from @c std::false_type and @c is_reference_v<Type> is @c false
/// @tparam Input The input type to check if reference type
template <typename Input>
constexpr auto static_assert_is_reference_false() noexcept -> void {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_reference<Input>, std::false_type>);
  ASSERT_FALSE(std::is_reference_v<Input>);
}

/// @brief Assert @c is_reference<Input> derives from @c std::true_type and @c is_reference_v<Type> is @c true
/// @tparam Input The input type to check if reference type
template <typename Input>
constexpr auto static_assert_is_reference_true() noexcept -> void {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_reference<Input>, std::true_type>);
  ASSERT_TRUE(std::is_reference_v<Input>);
}

template <typename T>
class IsNonReferenceTest : public testing::Test {};

TYPED_TEST_SUITE(IsNonReferenceTest, non_reference_types, );

/// @test @c is_reference is derived from @c false_type and @c is_reference_v is @c false for types that are not
/// reference types
CONSTEXPR_TYPED_TEST(IsNonReferenceTest, WithNonReferenceTypes) {
  static_assert_is_reference_false<TypeParam>();
  static_assert_is_reference_false<TypeParam const>();
  static_assert_is_reference_false<TypeParam volatile>();
  static_assert_is_reference_false<TypeParam const volatile>();
}

template <typename T>
class IsReferenceTest : public testing::Test {};

TYPED_TEST_SUITE(IsReferenceTest, non_reference_types, );

/// @test @c is_reference is derived from @c true_type and @c is_reference_v is @c true for types that are reference
/// types
CONSTEXPR_TYPED_TEST(IsReferenceTest, WithReferenceTypes) {
  static_assert_is_reference_true<TypeParam&>();
  static_assert_is_reference_true<TypeParam const&>();
  static_assert_is_reference_true<TypeParam volatile&>();
  static_assert_is_reference_true<TypeParam const volatile&>();
  static_assert_is_reference_true<TypeParam&&>();
  static_assert_is_reference_true<TypeParam const&&>();
  static_assert_is_reference_true<TypeParam volatile&&>();
  static_assert_is_reference_true<TypeParam const volatile&&>();
}

}  // namespace
