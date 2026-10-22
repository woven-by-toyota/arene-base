// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/swap_rows.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/linalg/equal_elements.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"  // IWYU pragma: keep
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_list/apply_each.hpp"
#include "arene/base/type_list/filter.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/counting_wrapper.hpp"

// IWYU pragma: no_include "arene/base/mdspan/extents.hpp"
// IWYU pragma: no_include "arene/base/mdspan/layout.hpp"
// IWYU pragma: no_include "arene/base/mdspan/mdspan.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/make_tuple.hpp"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::linalg_testing::category;
using arene::base::linalg_testing::test_array;
using arene::base::linalg_testing::unpack_param;
using arene::base::linalg_testing::unqualified_mdspan_types;
using arene::base::testing::mdspan_transform_element_type_t;
using arene::base::type_lists::apply_each_t;
using arene::base::type_lists::filter_t;
using linalg::swap_rows;
using testing::disable;
using testing::throws_on;

using swap_rows_fn = decltype(swap_rows);

template <typename T>
using to_counting_wrapper = ::testing::counting_wrapper<T>;

/// @brief @c Mdspan with its element type wrapped in a @c counting_wrapper so that calls to its swap can be counted
template <typename Mdspan>
using counting_matrix_type = mdspan_transform_element_type_t<Mdspan, to_counting_wrapper>;

template <throws_on ThrowsOn = throws_on::nothing, disable Disable = disable::nothing>
struct to_configurable_value {
  template <typename T>
  using type = ::testing::configurable_value<T, ThrowsOn, Disable>;
};

/// @brief A typed test fixture covering representative writeable @c mdspan types across ranks 0-3, static and dynamic
template <typename T>
class SwapRowsTypedTest : public ::testing::Test {};

/// @brief Check if a @c param is expected to be an inout matrix; used for filtering the big list to just inout matrices
template <typename T>
struct should_be_inout_matrix : std::integral_constant<bool, T::expectation == category::inout_matrix> {};

using inout_matrix_types = apply_each_t<filter_t<unqualified_mdspan_types, should_be_inout_matrix>, unpack_param>;

TYPED_TEST_SUITE(SwapRowsTypedTest, inout_matrix_types, );

/// @test @c swap_rows is invocable only if both row index arguments are possibly in-bounds
TYPED_TEST(SwapRowsTypedTest, InvocableForPossiblyInBoundsIndexTypes) {
  using arene::base::is_invocable_v;

  using dynamic_index = typename TypeParam::index_type;
  using in_bounds_index = std::integral_constant<dynamic_index, dynamic_index{}>;

  EXPECT_TRUE((is_invocable_v<swap_rows_fn, TypeParam, dynamic_index, dynamic_index>));
  EXPECT_TRUE((is_invocable_v<swap_rows_fn, TypeParam, dynamic_index, in_bounds_index>));
  EXPECT_TRUE((is_invocable_v<swap_rows_fn, TypeParam, in_bounds_index, dynamic_index>));
  EXPECT_TRUE((is_invocable_v<swap_rows_fn, TypeParam, in_bounds_index, in_bounds_index>));
}

/// @test @c swap_rows is invocable only if the matrix's value type is swappable
TYPED_TEST(SwapRowsTypedTest, NotInvocableWhenValueTypeNotSwappable) {
  using arene::base::is_invocable_v;
  using dynamic_index = typename TypeParam::index_type;

  // Start with a sanity check showing that just wrapping in configurable_value does not affect invocability.
  using all_enabled = mdspan_transform_element_type_t<TypeParam, to_configurable_value<>::type>;
  EXPECT_TRUE((is_invocable_v<swap_rows_fn, all_enabled, dynamic_index, dynamic_index>));

  // Even when there's no specific swap function, swap_rows is still invocable using the default swap.
  using no_swap =
      mdspan_transform_element_type_t<TypeParam, to_configurable_value<throws_on::nothing, disable::swap>::type>;
  EXPECT_TRUE(arene::base::is_swappable_v<typename no_swap::value_type>);
  EXPECT_TRUE((is_invocable_v<swap_rows_fn, no_swap, dynamic_index, dynamic_index>));

  // With both the moves and the custom swap disabled, the type is not swappable and swap_rows is no longer invocable.
  using no_swap_or_move = mdspan_transform_element_type_t<
      TypeParam,
      to_configurable_value<throws_on::nothing, disable::move_construct | disable::move_assign | disable::swap>::type>;
  EXPECT_FALSE(arene::base::is_swappable_v<typename no_swap_or_move::value_type>);
  EXPECT_FALSE((is_invocable_v<swap_rows_fn, no_swap_or_move, dynamic_index, dynamic_index>));

  // Disabling just the moves and leaving the custom swap in place means that the element type is still swappable, but
  // swap_rows is disabled in this case as well due to the element type no longer being a scalar.
  using no_move = mdspan_transform_element_type_t<
      TypeParam,
      to_configurable_value<throws_on::nothing, disable::move_construct | disable::move_assign>::type>;
  EXPECT_TRUE(arene::base::is_swappable_v<typename no_move::value_type>);
  EXPECT_FALSE(arene::base::linalg::is_scalar_v<typename no_move::value_type>);
  EXPECT_FALSE((is_invocable_v<swap_rows_fn, no_move, dynamic_index, dynamic_index>));
}

/// @test The noexcept specification of @c swap_rows is inherited from that of the elements' swap function
TYPED_TEST(SwapRowsTypedTest, NoexceptMatchesSwapFunction) {
  using arene::base::is_invocable_v;
  using arene::base::is_nothrow_invocable_v;
  using dynamic_index = typename TypeParam::index_type;

  // swap_rows is noexcept when invoked in an mdspan whose element type is just unconditionally noexcept.
  using all_noexcept = mdspan_transform_element_type_t<TypeParam, to_configurable_value<>::type>;
  EXPECT_TRUE((is_nothrow_invocable_v<swap_rows_fn, all_noexcept, dynamic_index, dynamic_index>));

  // swap_rows is still noexcept when swap is the only non-throwing operation.
  constexpr auto throws_on_all_but_swap = static_cast<throws_on>(+throws_on::everything & ~(+throws_on::swap));
  using only_swap_noexcept =
      mdspan_transform_element_type_t<TypeParam, to_configurable_value<throws_on_all_but_swap>::type>;
  EXPECT_TRUE(arene::base::is_nothrow_swappable_v<typename only_swap_noexcept::value_type>);
  EXPECT_TRUE((is_nothrow_invocable_v<swap_rows_fn, only_swap_noexcept, dynamic_index, dynamic_index>));

  // Conversely, swap_rows is noexcept(false) when swap is noexcept(false), even if everything else is safe.
  using only_swap_throws = mdspan_transform_element_type_t<TypeParam, to_configurable_value<throws_on::swap>::type>;
  EXPECT_TRUE(arene::base::is_swappable_v<typename only_swap_throws::value_type>);
  EXPECT_FALSE(arene::base::is_nothrow_swappable_v<typename only_swap_throws::value_type>);
  EXPECT_TRUE((is_invocable_v<swap_rows_fn, only_swap_throws, dynamic_index, dynamic_index>));
  EXPECT_FALSE((is_nothrow_invocable_v<swap_rows_fn, only_swap_throws, dynamic_index, dynamic_index>));
}

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "This test does not perform arithmetic so exact equality is correct");

/// @test @c swap_rows swaps the contents of the two indicated rows
ARENE_TYPED_PROPERTY(
    SwapRowsTypedTest,
    ContentsActuallySwapped,
    (test_array<TypeParam> matrix_storage,
     typename TypeParam::index_type index_i,
     typename TypeParam::index_type index_j)
) {
  TypeParam matrix{matrix_storage};
  if (matrix.extent(0) == typename TypeParam::index_type{}) {
    // The matrix must have at least one row for any part of this test to make sense
    GTEST_SKIP();
  }

  test_array<TypeParam> original_contents{matrix_storage};
  TypeParam original_matrix{original_contents};

  index_i = arene::base::abs(index_i) % matrix.extent(0);
  index_j = arene::base::abs(index_j) % matrix.extent(0);

  swap_rows(matrix, index_i, index_j);
  if (index_i != index_j) {
    for (typename TypeParam::index_type col_idx{}; col_idx < matrix.extent(1); ++col_idx) {
      ARENE_PROP_ASSERT(matrix(index_i, col_idx) == original_matrix(index_j, col_idx));
      ARENE_PROP_ASSERT(matrix(index_j, col_idx) == original_matrix(index_i, col_idx));
    }
  } else {
    ARENE_PROP_ASSERT(arene::base::linalg::equal_elements(matrix, original_matrix));
  }
}

ARENE_IGNORE_END();

/// @test @c swap_rows swaps elements using their custom swap function when one is present
ARENE_TYPED_PROPERTY(
    SwapRowsTypedTest,
    CallsCustomSwap,
    (test_array<counting_matrix_type<TypeParam>> matrix_storage,
     typename TypeParam::index_type index_i,
     typename TypeParam::index_type index_j)
) {
  using matrix_type = counting_matrix_type<TypeParam>;
  using index_type = typename matrix_type::index_type;
  using value_type = typename matrix_type::value_type;

  matrix_type const matrix{matrix_storage};
  if (matrix.extent(0) == index_type{}) {
    // The matrix must have at least one row for any part of this test to make sense
    GTEST_SKIP();
  }

  index_i = arene::base::abs(index_i) % matrix.extent(0);
  index_j = arene::base::abs(index_j) % matrix.extent(0);

  // If there are at least two distinct rows, then we can test what happens when a swap is actually performed
  if (matrix.extent(0) >= index_type{2}) {
    // In order for the swap to actually do anything, indices i and j must be distinct.
    if (index_j == index_i) {
      index_j = (index_i + index_type{1}) % matrix.extent(0);
    }

    value_type::count = {};
    swap_rows(matrix, index_i, index_j);
    ARENE_PROP_ASSERT(value_type::count.swap == matrix.extent(1));
  }

  // Using the same argument for both indices results in no calls to swap
  value_type::count = {};
  swap_rows(matrix, index_i, index_i);
  ARENE_PROP_ASSERT(value_type::count == decltype(value_type::count){});
}

template <typename T>
class SwapRowsNonInoutMatrixTest : public ::testing::Test {};

/// @brief Check if a @c param is expected to not be an inout matrix, including other mdspans, scalars, and other types
template <typename T>
struct should_not_be_inout_matrix : std::integral_constant<bool, T::expectation != category::inout_matrix> {};

using non_inout_matrix_types =
    apply_each_t<filter_t<unqualified_mdspan_types, should_not_be_inout_matrix>, unpack_param>;

TYPED_TEST_SUITE(SwapRowsNonInoutMatrixTest, non_inout_matrix_types, );

/// @test @c swap_rows is not invocable when the first argument is not an inout matrix
TYPED_TEST(SwapRowsNonInoutMatrixTest, OnlyInvocableForInoutMatrix) {
  using arene::base::is_invocable_v;

  EXPECT_FALSE((is_invocable_v<swap_rows_fn, TypeParam, std::size_t, std::size_t>));
}

}  // namespace
