// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "arene/base/type_list/remove_duplicates.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/mutate.hpp"

namespace {

using arene::base::type_lists::concat_unique_t;
using arene::base::type_lists::remove_duplicates_t;
using testing::is_unambiguously_publicly_derived_from_v;

using non_member_function_pointer_types = remove_duplicates_t<::testing::extend_with_cv_qualifiers_t<concat_unique_t<
    ::testing::arithmetic_types,
    ::testing::array_types,
    ::testing::class_types,
    ::testing::enum_types,
    ::testing::function_types,
    ::testing::member_object_pointer_types,
    ::testing::null_pointer_types,
    ::testing::pointer_types,
    ::testing::reference_types,
    ::testing::union_types>>>;

/// @brief Assert @c is_member_function_pointer<Input> derives from @c std::false_type and @c
/// is_member_function_pointer_v<Type> is @c false
/// @tparam Input The input type to check if member pointer type
template <typename Input>
constexpr auto static_assert_is_member_function_pointer_false() noexcept -> void {
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_member_function_pointer<Input>, std::false_type>,
      "is_member_function_pointer not false_type"
  );
  static_assert(!std::is_member_function_pointer_v<Input>, "is_member_function_pointer_v not false");
}

/// @brief Assert @c is_member_function_pointer<Input> derives from @c std::true_type and @c
/// is_member_function_pointer_v<Type> is @c true
/// @tparam Input The input type to check if member pointer type
template <typename Input>
constexpr auto static_assert_is_member_function_pointer_true() noexcept -> void {
  static_assert(
      is_unambiguously_publicly_derived_from_v<std::is_member_function_pointer<Input>, std::true_type>,
      "is_member_function_pointer not true_type"
  );
  static_assert(std::is_member_function_pointer_v<Input>, "is_member_function_pointer_v not true");
}

template <typename T>
class IsNonMemberFunctionPointerTest : public testing::Test {};

TYPED_TEST_SUITE(IsNonMemberFunctionPointerTest, non_member_function_pointer_types, );

/// @test @c is_member_function_pointer is derived from @c false_type and @c is_member_function_pointer_v is @c false
/// for types that are not member pointer types
TYPED_TEST(IsNonMemberFunctionPointerTest, WithNonMemberFunctionPointerTypes) {
  static_assert_is_member_function_pointer_false<TypeParam>();
}

using member_function_pointer_types = ::testing::extend_with_cv_qualifiers_t<::testing::member_function_pointer_types>;

template <typename T>
class IsMemberFunctionPointerTest : public testing::Test {};

TYPED_TEST_SUITE(IsMemberFunctionPointerTest, member_function_pointer_types, );

/// @test @c is_member_function_pointer is derived from @c true_type and @c is_member_function_pointer_v is @c true for
/// types that are member pointer types
TYPED_TEST(IsMemberFunctionPointerTest, WithMemberFunctionPointerTypes) {
  static_assert_is_member_function_pointer_true<TypeParam>();
  static_assert_is_member_function_pointer_false<TypeParam&>();
  static_assert_is_member_function_pointer_false<TypeParam&&>();
}

}  // namespace
