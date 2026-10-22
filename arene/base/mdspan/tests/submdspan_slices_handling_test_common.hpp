// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_SUBMDSPAN_SLICES_HANDLING_TEST_COMMON_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_SUBMDSPAN_SLICES_HANDLING_TEST_COMMON_HPP_

#include <gtest/gtest.h>

#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/mdspan/tests/throwing_slice_conversions.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

struct int_pair_like {
  template <std::size_t>
  constexpr auto get() const -> int {
    return 0;
  }
};

template <>
class std::tuple_size<int_pair_like> : public std::integral_constant<std::size_t, 2> {};

template <std::size_t I>
class std::tuple_element<I, int_pair_like> {
 public:
  using type = int;
};

namespace {

using arene::base::dextents;
using arene::base::extent_slice;
using arene::base::extents;
using arene::base::full_extent;
using arene::base::full_extent_t;
using arene::base::range_slice;

/// @brief implementation function for testing invocability of a function under test
/// @tparam F function under test
/// @tparam I extents index type
/// @note We don't use TYPED_TEST_P here since we would need *two* levels of
///   type parameterization
///
/// @test @c submdspan_canonicalize_slices or @c submdspan_extents is not
/// invocable if the number of slices does not match the rank of the extents
template <class F, class I>
auto static_assert_invocable_if_same_size() -> void {
  using ::arene::base::is_invocable_v;
  using index_type = I;

  static_assert(       //
      is_invocable_v<  //
          F,
          dextents<index_type, 0>>,
      "must be invocable"
  );
  static_assert(       //
      is_invocable_v<  //
          F,
          dextents<index_type, 1>,
          full_extent_t>,
      "must be invocable"
  );
  static_assert(       //
      is_invocable_v<  //
          F,
          dextents<index_type, 2>,
          full_extent_t,
          full_extent_t>,
      "must be invocable"
  );
  static_assert(       //
      is_invocable_v<  //
          F,
          dextents<index_type, 3>,
          full_extent_t,
          full_extent_t,
          full_extent_t>,
      "must be invocable"
  );

  static_assert(        //
      !is_invocable_v<  //
          F,
          dextents<index_type, 1>>,
      "must not be invocable"
  );
  static_assert(        //
      !is_invocable_v<  //
          F,
          dextents<index_type, 1>,
          full_extent_t,
          full_extent_t>,
      "must not be invocable"
  );
  static_assert(        //
      !is_invocable_v<  //
          F,
          dextents<index_type, 1>,
          full_extent_t,
          full_extent_t,
          full_extent_t>,
      "must not be invocable"
  );

  static_assert(        //
      !is_invocable_v<  //
          F,
          dextents<index_type, 3>>,
      "must not be invocable"
  );
  static_assert(        //
      !is_invocable_v<  //
          F,
          dextents<index_type, 3>,
          full_extent_t>,
      "must not be invocable"
  );
  static_assert(        //
      !is_invocable_v<  //
          F,
          dextents<index_type, 3>,
          full_extent_t,
          full_extent_t>,
      "must not be invocable"
  );
}

template <class T>
class SubmdspanSliceHandlingCommon : public ::testing::Test {};

TYPED_TEST_SUITE_P(SubmdspanSliceHandlingCommon);

/// @test @c submdspan_canonicalize_slices and @c submdspan_extents are @c noexcept(false) if
/// canonicalization of any slice can throw
TYPED_TEST_P(SubmdspanSliceHandlingCommon, Noexcept) {
  using ::arene::base::is_nothrow_invocable_v;

  auto const is_noexcept_with = [](auto... args) {
    static_assert(
        arene::base::is_invocable_v<decltype(TypeParam::function_under_test), decltype(args)...>,
        "must be invocable"
    );
    return arene::base::is_nothrow_invocable_v<decltype(TypeParam::function_under_test), decltype(args)...>;
  };

  ASSERT_TRUE(is_noexcept_with(full_extent, full_extent));
  ASSERT_TRUE(is_noexcept_with(int{}, int{}));

  ASSERT_FALSE(is_noexcept_with(arene::base::testing::throwing_convertible_to_full_extent{}, int{}));
  ASSERT_FALSE(is_noexcept_with(int{}, arene::base::testing::throwing_convertible_to_full_extent{}));

  ASSERT_FALSE(is_noexcept_with(arene::base::testing::throwing_convertible_to_int{}, int{}));
  ASSERT_FALSE(is_noexcept_with(int{}, arene::base::testing::throwing_convertible_to_int{}));

  ASSERT_TRUE(is_noexcept_with(std::pair<int, int>{}, int{}));

  ASSERT_FALSE(is_noexcept_with(std::pair<int, arene::base::testing::throwing_convertible_to_int>{}, int{}));
  ASSERT_FALSE(is_noexcept_with(std::pair<arene::base::testing::throwing_convertible_to_int, int>{}, int{}));
}

REGISTER_TYPED_TEST_SUITE_P(SubmdspanSliceHandlingCommon, Noexcept);

}  // namespace
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_TESTS_SUBMDSPAN_SLICES_HANDLING_TEST_COMMON_HPP_
