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
struct some_user_type {
  static void static_func();
  static void variadic_static_func(...);
  static void noexcept_static_func() noexcept;
  static void variadic_noexcept_static_func(...) noexcept;
  void operator()() {}
  void operator()(...) {}
};

using non_function_types = arene::base::type_lists::concat_unique_t<
    ::testing::array_types,
    ::testing::class_types,
    ::testing::enum_types,
    ::testing::member_pointer_types,
    ::testing::pointer_types,
    ::testing::reference_types,
    ::testing::scalar_types,
    ::testing::union_types>;

using function_types = arene::base::type_lists::concat_unique_t<
    ::testing::function_types,
    ::testing::Types<
        void(...),
        void() noexcept,
        void(...) noexcept,
        int(int) noexcept,
        decltype(some_user_type::static_func),
        decltype(some_user_type::variadic_static_func),
        decltype(some_user_type::noexcept_static_func),
        decltype(some_user_type::variadic_noexcept_static_func),
        some_user_type(),
        some_user_type(int)>>;

/// @brief Assert @c is_function<Input> derives from @c std::false_type and @c is_function_v<Type> is @c false
/// @tparam Input The input type to check if function type
template <typename Input>
constexpr auto static_assert_is_function_false() noexcept -> void {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_function<Input>, std::false_type>);
  ASSERT_FALSE(std::is_function_v<Input>);
}

/// @brief Assert @c is_function<Input> derives from @c std::true_type and @c is_function_v<Type> is @c true
/// @tparam Input The input type to check if function type
template <typename Input>
constexpr auto static_assert_is_function_true() noexcept -> void {
  ASSERT_TRUE(is_unambiguously_publicly_derived_from_v<std::is_function<Input>, std::true_type>);
  ASSERT_TRUE(std::is_function_v<Input>);
}

template <typename T>
class IsNonFunctionTest : public testing::Test {};

TYPED_TEST_SUITE(IsNonFunctionTest, non_function_types, );

/// @test @c is_function is derived from @c false_type and @c is_function_v is @c false for types that are not function
/// types
CONSTEXPR_TYPED_TEST(IsNonFunctionTest, WithNonFunctionTypes) {
  static_assert_is_function_false<TypeParam>();
  static_assert_is_function_false<TypeParam const>();
  static_assert_is_function_false<TypeParam volatile>();
  static_assert_is_function_false<TypeParam const volatile>();
  static_assert_is_function_false<TypeParam&>();
  static_assert_is_function_false<TypeParam const&>();
  static_assert_is_function_false<TypeParam volatile&>();
  static_assert_is_function_false<TypeParam const volatile&>();
  static_assert_is_function_false<TypeParam&&>();
  static_assert_is_function_false<TypeParam const&&>();
  static_assert_is_function_false<TypeParam volatile&&>();
  static_assert_is_function_false<TypeParam const volatile&&>();
}

template <typename T>
class IsFunctionTest : public testing::Test {};

TYPED_TEST_SUITE(IsFunctionTest, function_types, );

/// @test @c is_function is derived from @c true_type and @c is_function_v is @c true for function types
CONSTEXPR_TYPED_TEST(IsFunctionTest, WithFunctionTypes) {
  static_assert_is_function_true<TypeParam>();
  static_assert_is_function_true<TypeParam const>();
  static_assert_is_function_true<TypeParam volatile>();
  static_assert_is_function_true<TypeParam const volatile>();
}

/// @test @c is_function is derived from @c false_type and @c is_function_v is @c false for function reference types
CONSTEXPR_TYPED_TEST(IsFunctionTest, WithReferenceFunctionTypes) {
  static_assert_is_function_false<TypeParam&>();
  static_assert_is_function_false<TypeParam const&>();
  static_assert_is_function_false<TypeParam volatile&>();
  static_assert_is_function_false<TypeParam const volatile&>();
  static_assert_is_function_false<TypeParam&&>();
  static_assert_is_function_false<TypeParam const&&>();
  static_assert_is_function_false<TypeParam volatile&&>();
  static_assert_is_function_false<TypeParam const volatile&&>();
}

}  // namespace
