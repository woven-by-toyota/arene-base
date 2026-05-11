// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"

// NOLINTBEGIN(hicpp-avoid-c-arrays)

namespace {

/// @brief Assert @c std::extent<T, N> publicly inherits from integral_constant<std::size_t, I>
/// @tparam I the expected extent value
/// @tparam T the (array) type to obtain the extent of
/// @tparam N the dimension to obtain the extent of -- this pack is only valid
///   with size 0 or 1, i.e. it's more of an optional parameter than a real
///   variadic parameter.
///
template <std::size_t I, class T, unsigned... N>
constexpr auto static_assert_extent() noexcept -> void {
  static_assert(I == std::extent_v<T, N...>, "");

  testing::StaticAssertTypeEq<std::size_t const, decltype(std::extent_v<T, N...>)>();

  static_assert(
      testing::is_unambiguously_publicly_derived_from_v<  //
          std::extent<T, N...>,
          std::integral_constant<std::size_t, I>>,
      ""
  );
}

template <class T>
struct Extent : ::testing::Test {};

struct user_defined_type {};

using array_value_types = testing::Types<int, char, double, user_defined_type>;

TYPED_TEST_SUITE(Extent, array_value_types, );

/// @test @c std::extent returns the array dimension if known or 0 if unknown
/// for array types with rank 1
TYPED_TEST(Extent, Rank1ArrayTypes) {
  // clang-format off
  static_assert_extent<0, TypeParam                 [ ]>();
  static_assert_extent<1, TypeParam                 [1]>();
  static_assert_extent<0, TypeParam const           [ ]>();
  static_assert_extent<2, TypeParam const           [2]>();
  static_assert_extent<0, TypeParam volatile        [ ]>();
  static_assert_extent<3, TypeParam volatile        [3]>();
  static_assert_extent<0, TypeParam const volatile  [ ]>();
  static_assert_extent<4, TypeParam const volatile  [4]>();
  static_assert_extent<0, TypeParam*                [ ]>();
  static_assert_extent<5, TypeParam*                [5]>();
  static_assert_extent<0, TypeParam const*          [ ]>();
  static_assert_extent<6, TypeParam const*          [6]>();
  static_assert_extent<0, TypeParam volatile*       [ ]>();
  static_assert_extent<7, TypeParam volatile*       [7]>();
  static_assert_extent<0, TypeParam const volatile* [ ]>();
  static_assert_extent<8, TypeParam const volatile* [8]>();

  static_assert_extent<0, TypeParam                 [ ], 0>();
  static_assert_extent<1, TypeParam                 [1], 0>();
  static_assert_extent<0, TypeParam const           [ ], 0>();
  static_assert_extent<2, TypeParam const           [2], 0>();
  static_assert_extent<0, TypeParam volatile        [ ], 0>();
  static_assert_extent<3, TypeParam volatile        [3], 0>();
  static_assert_extent<0, TypeParam const volatile  [ ], 0>();
  static_assert_extent<4, TypeParam const volatile  [4], 0>();
  static_assert_extent<0, TypeParam*                [ ], 0>();
  static_assert_extent<5, TypeParam*                [5], 0>();
  static_assert_extent<0, TypeParam const*          [ ], 0>();
  static_assert_extent<6, TypeParam const*          [6], 0>();
  static_assert_extent<0, TypeParam volatile*       [ ], 0>();
  static_assert_extent<7, TypeParam volatile*       [7], 0>();
  static_assert_extent<0, TypeParam const volatile* [ ], 0>();
  static_assert_extent<8, TypeParam const volatile* [8], 0>();

  static_assert_extent<0, TypeParam                 [ ], 1>();
  static_assert_extent<0, TypeParam                 [1], 1>();
  static_assert_extent<0, TypeParam const           [ ], 1>();
  static_assert_extent<0, TypeParam const           [2], 1>();
  static_assert_extent<0, TypeParam volatile        [ ], 1>();
  static_assert_extent<0, TypeParam volatile        [3], 1>();
  static_assert_extent<0, TypeParam const volatile  [ ], 1>();
  static_assert_extent<0, TypeParam const volatile  [4], 1>();
  static_assert_extent<0, TypeParam*                [ ], 1>();
  static_assert_extent<0, TypeParam*                [5], 1>();
  static_assert_extent<0, TypeParam const*          [ ], 1>();
  static_assert_extent<0, TypeParam const*          [6], 1>();
  static_assert_extent<0, TypeParam volatile*       [ ], 1>();
  static_assert_extent<0, TypeParam volatile*       [7], 1>();
  static_assert_extent<0, TypeParam const volatile* [ ], 1>();
  static_assert_extent<0, TypeParam const volatile* [8], 1>();
  // clang-format on
}

/// @test @c std::extent returns the array dimension if known or 0 if unknown
/// for array types with rank 2
TYPED_TEST(Extent, Rank2ArrayTypes) {
  // NOTE
  // only the first dimension of an array can be unknown
  // https://eel.is/c++draft/dcl.array#9

  // clang-format off
  static_assert_extent<0, TypeParam                 [ ][9]>();
  static_assert_extent<1, TypeParam                 [1][8]>();
  static_assert_extent<0, TypeParam                 [ ][7]>();
  static_assert_extent<2, TypeParam                 [2][6]>();
  static_assert_extent<0, TypeParam const           [ ][5]>();
  static_assert_extent<3, TypeParam const           [3][4]>();
  static_assert_extent<0, TypeParam volatile        [ ][3]>();
  static_assert_extent<4, TypeParam volatile        [4][2]>();
  static_assert_extent<0, TypeParam const volatile  [ ][1]>();
  static_assert_extent<5, TypeParam const volatile  [5][9]>();
  static_assert_extent<0, TypeParam*                [ ][8]>();
  static_assert_extent<6, TypeParam*                [6][7]>();
  static_assert_extent<0, TypeParam const*          [ ][6]>();
  static_assert_extent<7, TypeParam const*          [7][5]>();
  static_assert_extent<0, TypeParam volatile*       [ ][4]>();
  static_assert_extent<8, TypeParam volatile*       [8][3]>();
  static_assert_extent<0, TypeParam const volatile* [ ][2]>();
  static_assert_extent<9, TypeParam const volatile* [9][1]>();

  static_assert_extent<0, TypeParam                 [ ][9], 0>();
  static_assert_extent<1, TypeParam                 [1][8], 0>();
  static_assert_extent<0, TypeParam                 [ ][7], 0>();
  static_assert_extent<2, TypeParam                 [2][6], 0>();
  static_assert_extent<0, TypeParam const           [ ][5], 0>();
  static_assert_extent<3, TypeParam const           [3][4], 0>();
  static_assert_extent<0, TypeParam volatile        [ ][3], 0>();
  static_assert_extent<4, TypeParam volatile        [4][2], 0>();
  static_assert_extent<0, TypeParam const volatile  [ ][1], 0>();
  static_assert_extent<5, TypeParam const volatile  [5][9], 0>();
  static_assert_extent<0, TypeParam*                [ ][8], 0>();
  static_assert_extent<6, TypeParam*                [6][7], 0>();
  static_assert_extent<0, TypeParam const*          [ ][6], 0>();
  static_assert_extent<7, TypeParam const*          [7][5], 0>();
  static_assert_extent<0, TypeParam volatile*       [ ][4], 0>();
  static_assert_extent<8, TypeParam volatile*       [8][3], 0>();
  static_assert_extent<0, TypeParam const volatile* [ ][2], 0>();
  static_assert_extent<9, TypeParam const volatile* [9][1], 0>();

  static_assert_extent<9, TypeParam                 [ ][9], 1>();
  static_assert_extent<8, TypeParam                 [1][8], 1>();
  static_assert_extent<7, TypeParam                 [ ][7], 1>();
  static_assert_extent<6, TypeParam                 [2][6], 1>();
  static_assert_extent<5, TypeParam const           [ ][5], 1>();
  static_assert_extent<4, TypeParam const           [3][4], 1>();
  static_assert_extent<3, TypeParam volatile        [ ][3], 1>();
  static_assert_extent<2, TypeParam volatile        [4][2], 1>();
  static_assert_extent<1, TypeParam const volatile  [ ][1], 1>();
  static_assert_extent<9, TypeParam const volatile  [5][9], 1>();
  static_assert_extent<8, TypeParam*                [ ][8], 1>();
  static_assert_extent<7, TypeParam*                [6][7], 1>();
  static_assert_extent<6, TypeParam const*          [ ][6], 1>();
  static_assert_extent<5, TypeParam const*          [7][5], 1>();
  static_assert_extent<4, TypeParam volatile*       [ ][4], 1>();
  static_assert_extent<3, TypeParam volatile*       [8][3], 1>();
  static_assert_extent<2, TypeParam const volatile* [ ][2], 1>();
  static_assert_extent<1, TypeParam const volatile* [9][1], 1>();

  static_assert_extent<0, TypeParam                 [ ][9], 2>();
  static_assert_extent<0, TypeParam                 [1][8], 2>();
  static_assert_extent<0, TypeParam                 [ ][7], 2>();
  static_assert_extent<0, TypeParam                 [2][6], 2>();
  static_assert_extent<0, TypeParam const           [ ][5], 2>();
  static_assert_extent<0, TypeParam const           [3][4], 2>();
  static_assert_extent<0, TypeParam volatile        [ ][3], 2>();
  static_assert_extent<0, TypeParam volatile        [4][2], 2>();
  static_assert_extent<0, TypeParam const volatile  [ ][1], 2>();
  static_assert_extent<0, TypeParam const volatile  [5][9], 2>();
  static_assert_extent<0, TypeParam*                [ ][8], 2>();
  static_assert_extent<0, TypeParam*                [6][7], 2>();
  static_assert_extent<0, TypeParam const*          [ ][6], 2>();
  static_assert_extent<0, TypeParam const*          [7][5], 2>();
  static_assert_extent<0, TypeParam volatile*       [ ][4], 2>();
  static_assert_extent<0, TypeParam volatile*       [8][3], 2>();
  static_assert_extent<0, TypeParam const volatile* [ ][2], 2>();
  static_assert_extent<0, TypeParam const volatile* [9][1], 2>();
  // clang-format on
}

/// @test @c std::extent returns the array dimension if known or 0 if unknown
/// for array types with rank 3
TYPED_TEST(Extent, Rank3ArrayTypes) {
  // NOTE
  // only the first dimension of an array can be unknown
  // https://eel.is/c++draft/dcl.array#9

  // clang-format off
  static_assert_extent<0, TypeParam                 [ ][9][11]>();
  static_assert_extent<1, TypeParam                 [1][8][12]>();
  static_assert_extent<0, TypeParam                 [ ][7][13]>();
  static_assert_extent<2, TypeParam                 [2][6][14]>();
  static_assert_extent<0, TypeParam const           [ ][5][15]>();
  static_assert_extent<3, TypeParam const           [3][4][16]>();
  static_assert_extent<0, TypeParam volatile        [ ][3][17]>();
  static_assert_extent<4, TypeParam volatile        [4][2][18]>();
  static_assert_extent<0, TypeParam const volatile  [ ][1][19]>();
  static_assert_extent<5, TypeParam const volatile  [5][9][11]>();
  static_assert_extent<0, TypeParam*                [ ][8][12]>();
  static_assert_extent<6, TypeParam*                [6][7][13]>();
  static_assert_extent<0, TypeParam const*          [ ][6][14]>();
  static_assert_extent<7, TypeParam const*          [7][5][15]>();
  static_assert_extent<0, TypeParam volatile*       [ ][4][16]>();
  static_assert_extent<8, TypeParam volatile*       [8][3][17]>();
  static_assert_extent<0, TypeParam const volatile* [ ][2][18]>();
  static_assert_extent<9, TypeParam const volatile* [9][1][19]>();

  static_assert_extent<0, TypeParam                 [ ][9][11], 0>();
  static_assert_extent<1, TypeParam                 [1][8][12], 0>();
  static_assert_extent<0, TypeParam                 [ ][7][13], 0>();
  static_assert_extent<2, TypeParam                 [2][6][14], 0>();
  static_assert_extent<0, TypeParam const           [ ][5][15], 0>();
  static_assert_extent<3, TypeParam const           [3][4][16], 0>();
  static_assert_extent<0, TypeParam volatile        [ ][3][17], 0>();
  static_assert_extent<4, TypeParam volatile        [4][2][18], 0>();
  static_assert_extent<0, TypeParam const volatile  [ ][1][19], 0>();
  static_assert_extent<5, TypeParam const volatile  [5][9][11], 0>();
  static_assert_extent<0, TypeParam*                [ ][8][12], 0>();
  static_assert_extent<6, TypeParam*                [6][7][13], 0>();
  static_assert_extent<0, TypeParam const*          [ ][6][14], 0>();
  static_assert_extent<7, TypeParam const*          [7][5][15], 0>();
  static_assert_extent<0, TypeParam volatile*       [ ][4][16], 0>();
  static_assert_extent<8, TypeParam volatile*       [8][3][17], 0>();
  static_assert_extent<0, TypeParam const volatile* [ ][2][18], 0>();
  static_assert_extent<9, TypeParam const volatile* [9][1][19], 0>();

  static_assert_extent<9, TypeParam                 [ ][9][11], 1>();
  static_assert_extent<8, TypeParam                 [1][8][12], 1>();
  static_assert_extent<7, TypeParam                 [ ][7][13], 1>();
  static_assert_extent<6, TypeParam                 [2][6][14], 1>();
  static_assert_extent<5, TypeParam const           [ ][5][15], 1>();
  static_assert_extent<4, TypeParam const           [3][4][16], 1>();
  static_assert_extent<3, TypeParam volatile        [ ][3][17], 1>();
  static_assert_extent<2, TypeParam volatile        [4][2][18], 1>();
  static_assert_extent<1, TypeParam const volatile  [ ][1][19], 1>();
  static_assert_extent<9, TypeParam const volatile  [5][9][11], 1>();
  static_assert_extent<8, TypeParam*                [ ][8][12], 1>();
  static_assert_extent<7, TypeParam*                [6][7][13], 1>();
  static_assert_extent<6, TypeParam const*          [ ][6][14], 1>();
  static_assert_extent<5, TypeParam const*          [7][5][15], 1>();
  static_assert_extent<4, TypeParam volatile*       [ ][4][16], 1>();
  static_assert_extent<3, TypeParam volatile*       [8][3][17], 1>();
  static_assert_extent<2, TypeParam const volatile* [ ][2][18], 1>();
  static_assert_extent<1, TypeParam const volatile* [9][1][19], 1>();

  static_assert_extent<11, TypeParam                 [ ][9][11], 2>();
  static_assert_extent<12, TypeParam                 [1][8][12], 2>();
  static_assert_extent<13, TypeParam                 [ ][7][13], 2>();
  static_assert_extent<14, TypeParam                 [2][6][14], 2>();
  static_assert_extent<15, TypeParam const           [ ][5][15], 2>();
  static_assert_extent<16, TypeParam const           [3][4][16], 2>();
  static_assert_extent<17, TypeParam volatile        [ ][3][17], 2>();
  static_assert_extent<18, TypeParam volatile        [4][2][18], 2>();
  static_assert_extent<19, TypeParam const volatile  [ ][1][19], 2>();
  static_assert_extent<11, TypeParam const volatile  [5][9][11], 2>();
  static_assert_extent<12, TypeParam*                [ ][8][12], 2>();
  static_assert_extent<13, TypeParam*                [6][7][13], 2>();
  static_assert_extent<14, TypeParam const*          [ ][6][14], 2>();
  static_assert_extent<15, TypeParam const*          [7][5][15], 2>();
  static_assert_extent<16, TypeParam volatile*       [ ][4][16], 2>();
  static_assert_extent<17, TypeParam volatile*       [8][3][17], 2>();
  static_assert_extent<18, TypeParam const volatile* [ ][2][18], 2>();
  static_assert_extent<19, TypeParam const volatile* [9][1][19], 2>();

  static_assert_extent<0, TypeParam                 [ ][9][11], 3>();
  static_assert_extent<0, TypeParam                 [1][8][12], 3>();
  static_assert_extent<0, TypeParam                 [ ][7][13], 3>();
  static_assert_extent<0, TypeParam                 [2][6][14], 3>();
  static_assert_extent<0, TypeParam const           [ ][5][15], 3>();
  static_assert_extent<0, TypeParam const           [3][4][16], 3>();
  static_assert_extent<0, TypeParam volatile        [ ][3][17], 3>();
  static_assert_extent<0, TypeParam volatile        [4][2][18], 3>();
  static_assert_extent<0, TypeParam const volatile  [ ][1][19], 3>();
  static_assert_extent<0, TypeParam const volatile  [5][9][11], 3>();
  static_assert_extent<0, TypeParam*                [ ][8][12], 3>();
  static_assert_extent<0, TypeParam*                [6][7][13], 3>();
  static_assert_extent<0, TypeParam const*          [ ][6][14], 3>();
  static_assert_extent<0, TypeParam const*          [7][5][15], 3>();
  static_assert_extent<0, TypeParam volatile*       [ ][4][16], 3>();
  static_assert_extent<0, TypeParam volatile*       [8][3][17], 3>();
  static_assert_extent<0, TypeParam const volatile* [ ][2][18], 3>();
  static_assert_extent<0, TypeParam const volatile* [9][1][19], 3>();
  // clang-format on
}

template <class T>
struct ExtentNonArrayTypes : ::testing::Test {};

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
        int const volatile (*)[1]>>;

TYPED_TEST_SUITE(ExtentNonArrayTypes, non_array_types, );

/// @test @c std::extent returns 0 for non-array types
TYPED_TEST(ExtentNonArrayTypes, AlwaysZero) {
  static_assert_extent<0, TypeParam>();
  static_assert_extent<0, TypeParam const>();
  static_assert_extent<0, TypeParam volatile>();
  static_assert_extent<0, TypeParam const volatile>();

  static_assert_extent<0, TypeParam, 0>();
  static_assert_extent<0, TypeParam const, 0>();
  static_assert_extent<0, TypeParam volatile, 0>();
  static_assert_extent<0, TypeParam const volatile, 0>();

  static_assert_extent<0, TypeParam, 1>();
  static_assert_extent<0, TypeParam const, 1>();
  static_assert_extent<0, TypeParam volatile, 1>();
  static_assert_extent<0, TypeParam const volatile, 1>();

  static_assert_extent<0, TypeParam, 2>();
  static_assert_extent<0, TypeParam const, 2>();
  static_assert_extent<0, TypeParam volatile, 2>();
  static_assert_extent<0, TypeParam const volatile, 2>();
}

}  // namespace

// NOLINTEND(hicpp-avoid-c-arrays)
