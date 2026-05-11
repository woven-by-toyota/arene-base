// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

// NOLINTBEGIN(hicpp-avoid-c-arrays)

namespace {

/// @brief Assert @c remove_extent<Input>::type is the same as @c ExpectedResult
/// @tparam Input The input type to remove an extent from
/// @tparam ExpectedResult The expected result of removing an extent from the @c Input type
template <typename Input, typename ExpectedResult>
constexpr auto static_assert_remove_extent() noexcept -> void {
  testing::StaticAssertTypeEq<typename std::remove_extent<Input>::type, ExpectedResult>();
  testing::StaticAssertTypeEq<std::remove_extent_t<Input>, ExpectedResult>();
}

template <typename T>
struct RemoveExtent : ::testing::Test {};

struct user_defined_type {};

using array_value_types = testing::Types<int, char, double, user_defined_type>;

TYPED_TEST_SUITE(RemoveExtent, array_value_types, );

/// @test @c remove_extent returns the object type for a rank 1 arrays
TYPED_TEST(RemoveExtent, Rank1ArrayTypes) {
  // clang-format off
  static_assert_remove_extent<TypeParam                 [ ], TypeParam                >();
  static_assert_remove_extent<TypeParam                 [ ], TypeParam                >();
  static_assert_remove_extent<TypeParam const           [1], TypeParam const          >();
  static_assert_remove_extent<TypeParam const           [1], TypeParam const          >();
  static_assert_remove_extent<TypeParam volatile        [ ], TypeParam volatile       >();
  static_assert_remove_extent<TypeParam volatile        [ ], TypeParam volatile       >();
  static_assert_remove_extent<TypeParam const volatile  [1], TypeParam const volatile >();
  static_assert_remove_extent<TypeParam const volatile  [1], TypeParam const volatile >();
  static_assert_remove_extent<TypeParam*                [ ], TypeParam*               >();
  static_assert_remove_extent<TypeParam*                [1], TypeParam*               >();
  static_assert_remove_extent<TypeParam const*          [ ], TypeParam const*         >();
  static_assert_remove_extent<TypeParam const*          [1], TypeParam const*         >();
  static_assert_remove_extent<TypeParam volatile*       [ ], TypeParam volatile*      >();
  static_assert_remove_extent<TypeParam volatile*       [1], TypeParam volatile*      >();
  static_assert_remove_extent<TypeParam const volatile* [ ], TypeParam const volatile*>();
  static_assert_remove_extent<TypeParam const volatile* [1], TypeParam const volatile*>();
  // clang-format on
}

/// @test @c remove_extent returns a rank 1 array with the first extent removed for rank 2 arrays
TYPED_TEST(RemoveExtent, Rank2ArrayTypes) {
  // NOTE
  // only the first dimension of an array can be unknown
  // https://eel.is/c++draft/dcl.array#9

  // clang-format off
  static_assert_remove_extent<TypeParam                 [ ][2], TypeParam                 [2]>();
  static_assert_remove_extent<TypeParam                 [1][2], TypeParam                 [2]>();
  static_assert_remove_extent<TypeParam                 [ ][2], TypeParam                 [2]>();
  static_assert_remove_extent<TypeParam                 [ ][2], TypeParam                 [2]>();
  static_assert_remove_extent<TypeParam const           [1][2], TypeParam const           [2]>();
  static_assert_remove_extent<TypeParam const           [1][2], TypeParam const           [2]>();
  static_assert_remove_extent<TypeParam volatile        [ ][2], TypeParam volatile        [2]>();
  static_assert_remove_extent<TypeParam volatile        [ ][2], TypeParam volatile        [2]>();
  static_assert_remove_extent<TypeParam const volatile  [1][2], TypeParam const volatile  [2]>();
  static_assert_remove_extent<TypeParam const volatile  [1][2], TypeParam const volatile  [2]>();
  static_assert_remove_extent<TypeParam*                [ ][2], TypeParam*                [2]>();
  static_assert_remove_extent<TypeParam*                [1][2], TypeParam*                [2]>();
  static_assert_remove_extent<TypeParam const*          [ ][2], TypeParam const*          [2]>();
  static_assert_remove_extent<TypeParam const*          [1][2], TypeParam const*          [2]>();
  static_assert_remove_extent<TypeParam volatile*       [ ][2], TypeParam volatile*       [2]>();
  static_assert_remove_extent<TypeParam volatile*       [1][2], TypeParam volatile*       [2]>();
  static_assert_remove_extent<TypeParam const volatile* [ ][2], TypeParam const volatile* [2]>();
  static_assert_remove_extent<TypeParam const volatile* [1][2], TypeParam const volatile* [2]>();
  // clang-format on
}

/// @test @c remove_extent returns a rank 2 array with the first extent removed for rank 3 arrays
TYPED_TEST(RemoveExtent, Rank3ArrayTypes) {
  // NOTE
  // only the first dimension of an array can be unknown
  // https://eel.is/c++draft/dcl.array#9

  // clang-format off
  static_assert_remove_extent<TypeParam                 [ ][2][3], TypeParam                 [2][3]>();
  static_assert_remove_extent<TypeParam                 [1][2][3], TypeParam                 [2][3]>();
  static_assert_remove_extent<TypeParam                 [ ][2][3], TypeParam                 [2][3]>();
  static_assert_remove_extent<TypeParam                 [ ][2][3], TypeParam                 [2][3]>();
  static_assert_remove_extent<TypeParam const           [1][2][3], TypeParam const           [2][3]>();
  static_assert_remove_extent<TypeParam const           [1][2][3], TypeParam const           [2][3]>();
  static_assert_remove_extent<TypeParam volatile        [ ][2][3], TypeParam volatile        [2][3]>();
  static_assert_remove_extent<TypeParam volatile        [ ][2][3], TypeParam volatile        [2][3]>();
  static_assert_remove_extent<TypeParam const volatile  [1][2][3], TypeParam const volatile  [2][3]>();
  static_assert_remove_extent<TypeParam const volatile  [1][2][3], TypeParam const volatile  [2][3]>();
  static_assert_remove_extent<TypeParam*                [ ][2][3], TypeParam*                [2][3]>();
  static_assert_remove_extent<TypeParam*                [1][2][3], TypeParam*                [2][3]>();
  static_assert_remove_extent<TypeParam const*          [ ][2][3], TypeParam const*          [2][3]>();
  static_assert_remove_extent<TypeParam const*          [1][2][3], TypeParam const*          [2][3]>();
  static_assert_remove_extent<TypeParam volatile*       [ ][2][3], TypeParam volatile*       [2][3]>();
  static_assert_remove_extent<TypeParam volatile*       [1][2][3], TypeParam volatile*       [2][3]>();
  static_assert_remove_extent<TypeParam const volatile* [ ][2][3], TypeParam const volatile* [2][3]>();
  static_assert_remove_extent<TypeParam const volatile* [1][2][3], TypeParam const volatile* [2][3]>();
  // clang-format on
}

template <typename T>
struct RemoveExtentNonArrayTypes : ::testing::Test {};

using non_array_types = arene::base::type_lists::concat_unique_t<
    ::testing::scalar_types,
    ::testing::function_types,
    ::testing::pointer_types,
    ::testing::member_pointer_types,
    ::testing::reference_types,
    ::testing::class_types,
    ::testing::union_types,
    ::testing::enum_types,
    ::testing::non_referenceable_types,
    ::testing::Types<
        int (&)[],
        int const (&)[],
        int volatile (&)[],
        int const volatile (&)[],
        int (&&)[],
        int const (&&)[],
        int volatile (&&)[],
        int const volatile (&&)[],
        int (*)[],
        int const (*)[],
        int volatile (*)[],
        int const volatile (*)[],
        int (&)[1],
        int const (&)[1],
        int volatile (&)[1],
        int const volatile (&)[1],
        int (&&)[1],
        int const (&&)[1],
        int volatile (&&)[1],
        int const volatile (&&)[1],
        int (*)[1],
        int const (*)[1],
        int volatile (*)[1],
        int const volatile (*)[1]> >;

TYPED_TEST_SUITE(RemoveExtentNonArrayTypes, non_array_types, );

/// @test @c remove_extent correctly returns the input type when not given an array
TYPED_TEST(RemoveExtentNonArrayTypes, ReturnsTheSameType) {
  static_assert_remove_extent<TypeParam, TypeParam>();
  static_assert_remove_extent<TypeParam const, TypeParam const>();
  static_assert_remove_extent<TypeParam volatile, TypeParam volatile>();
  static_assert_remove_extent<TypeParam const volatile, TypeParam const volatile>();
}

}  // namespace

// NOLINTEND(hicpp-avoid-c-arrays)
