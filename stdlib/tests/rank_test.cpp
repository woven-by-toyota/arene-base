// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_list/concat_unique.hpp"
#include "stdlib/include/type_traits"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/common_test_types.hpp"
#include "testlibs/utilities/mutate.hpp"

// NOLINTBEGIN(hicpp-avoid-c-arrays)

namespace {

/// @brief Assert @c std::rank<T> publicly inherits from integral_constant<std::size_t, I>
/// @tparam I the expected rank value
/// @tparam T the (array) type to obtain the rank of
///
template <std::size_t I, class T>
constexpr auto static_assert_rank() noexcept -> void {
  static_assert(
      testing::is_unambiguously_publicly_derived_from_v<  //
          std::rank<T>,
          std::integral_constant<std::size_t, I>>,
      ""
  );
  static_assert(I == std::rank_v<T>, "");
  testing::StaticAssertTypeEq<std::size_t const, decltype(std::rank_v<T>)>();
}

template <class T>
struct Rank : ::testing::Test {};

struct user_defined_type {};

using array_value_types = testing::Types<int, char, double, user_defined_type>;
using with_pointer_types = testing::mutate_t<array_value_types, ::testing::type_identity, std::add_pointer>;
using all_array_values_types = testing::extend_with_cv_qualifiers_t<with_pointer_types>;

TYPED_TEST_SUITE(Rank, all_array_values_types, );

/// @test @c std::rank returns the number of dimensions of an array type
TYPED_TEST(Rank, RankTest) {
  static_assert_rank<1, TypeParam[]>();
  static_assert_rank<1, TypeParam[9]>();

  static_assert_rank<2, TypeParam[][9]>();
  static_assert_rank<2, TypeParam[9][99]>();

  static_assert_rank<3, TypeParam[][9][99]>();
  static_assert_rank<3, TypeParam[9][99][999]>();

  static_assert_rank<10, TypeParam[][1][2][3][4][5][6][7][8][9]>();
  static_assert_rank<10, TypeParam[1][2][3][4][5][6][7][8][9][10]>();
}

template <class T>
struct RankNonArrayTypes : ::testing::Test {};

using non_array_types = arene::base::type_lists::concat_unique_t<
    ::testing::non_array_object_types,
    ::testing::function_types,
    ::testing::reference_types,
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

TYPED_TEST_SUITE(RankNonArrayTypes, non_array_types, );

/// @test @c std::rank returns 0 for non-array types
TYPED_TEST(RankNonArrayTypes, AlwaysZero) {
  static_assert_rank<0, TypeParam>();
  static_assert_rank<0, TypeParam const>();
  static_assert_rank<0, TypeParam volatile>();
  static_assert_rank<0, TypeParam const volatile>();
}

}  // namespace

// NOLINTEND(hicpp-avoid-c-arrays)
