// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

/// @brief A user-defined type with bracket operator
class bracket_user_type {
  template <typename T>
  void operator[](T) const {}
};

using array_capable_types = arene::base::type_lists::concat_unique_t<
    ::testing::class_types,
    ::testing::enum_types,
    ::testing::member_pointer_types,
    ::testing::pointer_types,
    ::testing::scalar_types,
    ::testing::union_types,
    ::testing::Types<bracket_user_type>>;
using non_array_types = arene::base::type_lists::
    concat_unique_t<array_capable_types, ::testing::function_types, ::testing::reference_types>;

/// @brief An arbitrary size for an array type
constexpr std::size_t k_array_size = 10;

/// @brief Assert @c is_array<Input> derives from @c std::false_type and @c is_array_v<Type> is @c false
/// @tparam Input The input type to check if array type
template <typename Input>
constexpr auto static_assert_is_array_false() noexcept -> void {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_array<Input>, std::false_type>);
  ASSERT_FALSE(std::is_array_v<Input>);
}

/// @brief Assert @c is_array<Input> derives from @c std::true_type and @c is_array_v<Type> is @c true
/// @tparam Input The input type to check if array type
template <typename Input>
constexpr auto static_assert_is_array_true() noexcept -> void {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_array<Input>, std::true_type>);
  ASSERT_TRUE(std::is_array_v<Input>);
}

template <typename T>
class IsNonArrayTest : public testing::Test {};

TYPED_TEST_SUITE(IsNonArrayTest, non_array_types, );

/// @test @c is_array is derived from @c false_type and @c is_array_v is @c false for types that are not array types
CONSTEXPR_TYPED_TEST(IsNonArrayTest, WithNonArrayTypes) {
  static_assert_is_array_false<TypeParam>();
  static_assert_is_array_false<TypeParam const>();
  static_assert_is_array_false<TypeParam volatile>();
  static_assert_is_array_false<TypeParam const volatile>();
  static_assert_is_array_false<TypeParam&>();
  static_assert_is_array_false<TypeParam const&>();
  static_assert_is_array_false<TypeParam volatile&>();
  static_assert_is_array_false<TypeParam const volatile&>();
  static_assert_is_array_false<TypeParam&&>();
  static_assert_is_array_false<TypeParam const&&>();
  static_assert_is_array_false<TypeParam volatile&&>();
  static_assert_is_array_false<TypeParam const volatile&&>();
}

template <typename T>
class IsArrayTest : public testing::Test {};

TYPED_TEST_SUITE(IsArrayTest, array_capable_types, );

/// @test @c is_array is derived from @c true_type and @c is_array_v is @c true for array types
CONSTEXPR_TYPED_TEST(IsArrayTest, WithArrayTypes) {
  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  static_assert_is_array_true<TypeParam[]>();
  static_assert_is_array_true<TypeParam const[]>();
  static_assert_is_array_true<TypeParam volatile[]>();
  static_assert_is_array_true<TypeParam const volatile[]>();
  static_assert_is_array_true<TypeParam[k_array_size]>();
  static_assert_is_array_true<TypeParam const[k_array_size]>();
  static_assert_is_array_true<TypeParam volatile[k_array_size]>();
  static_assert_is_array_true<TypeParam const volatile[k_array_size]>();
  // NOLINTEND(hicpp-avoid-c-arrays)
}

/// @test @c is_array is derived from @c false_type and @c is_array_v is @c false for reference to array types
CONSTEXPR_TYPED_TEST(IsArrayTest, WithArrayReferenceTypes) {
  // NOLINTBEGIN(hicpp-avoid-c-arrays)
  static_assert_is_array_false<TypeParam(&)[]>();
  static_assert_is_array_false<TypeParam const(&)[]>();
  static_assert_is_array_false<TypeParam volatile(&)[]>();
  static_assert_is_array_false<TypeParam const volatile(&)[]>();
  static_assert_is_array_false<TypeParam(&&)[]>();
  static_assert_is_array_false<TypeParam const(&&)[]>();
  static_assert_is_array_false<TypeParam volatile(&&)[]>();
  static_assert_is_array_false<TypeParam const volatile(&&)[]>();
  static_assert_is_array_false<TypeParam(&)[k_array_size]>();
  static_assert_is_array_false<TypeParam const(&)[k_array_size]>();
  static_assert_is_array_false<TypeParam volatile(&)[k_array_size]>();
  static_assert_is_array_false<TypeParam const volatile(&)[k_array_size]>();
  static_assert_is_array_false<TypeParam(&&)[k_array_size]>();
  static_assert_is_array_false<TypeParam const(&&)[k_array_size]>();
  static_assert_is_array_false<TypeParam volatile(&&)[k_array_size]>();
  static_assert_is_array_false<TypeParam const volatile(&&)[k_array_size]>();
  // NOLINTEND(hicpp-avoid-c-arrays)
}

}  // namespace
