// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/row_column_view.hpp"

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/equal_elements.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/mdspan/submdspan.hpp"
#include "arene/base/mdspan/tests/mdspan_test_utilities.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

// IWYU pragma: no_include "arene/base/stdlib_choice/tuple.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/make_tuple.hpp"

namespace {

using arene::base::full_extent;
using arene::base::submdspan;
using arene::base::linalg::column_view;
using arene::base::linalg::equal_elements;
using arene::base::linalg::row_view;
using arene::base::testing::equal_address;
using arene::base::testing::make_test_mdspan;

/// @test row_view and column_view are only invocable with rank-2 mdspans
TEST(RowColumnViewTest, RejectsNonMatrix) {
  using arene::base::is_invocable_v;
  using vector_t = arene::base::mdspan<int, arene::base::extents<std::size_t, 4>>;
  using tensor_t = arene::base::mdspan<int, arene::base::extents<std::size_t, 2, 3, 4>>;

  STATIC_ASSERT_FALSE((is_invocable_v<decltype(row_view), vector_t>));
  STATIC_ASSERT_FALSE((is_invocable_v<decltype(column_view), vector_t>));
  STATIC_ASSERT_FALSE((is_invocable_v<decltype(row_view), tensor_t>));
  STATIC_ASSERT_FALSE((is_invocable_v<decltype(column_view), tensor_t>));
  STATIC_ASSERT_FALSE((is_invocable_v<decltype(row_view), int>));
  STATIC_ASSERT_FALSE((is_invocable_v<decltype(column_view), int>));
}

template <class Matrix>
class RowColumnViewMatrixTypesTest : public ::testing::Test {};

TYPED_TEST_SUITE(RowColumnViewMatrixTypesTest, arene::base::linalg_testing::unqualified_linalg_matrix_types, );

/// @test row_view and column_view are invocable for any rank-2 in-matrix
TYPED_TEST(RowColumnViewMatrixTypesTest, IsInvocable) {
  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<decltype(row_view), TypeParam>));
  STATIC_ASSERT_TRUE((arene::base::is_invocable_v<decltype(column_view), TypeParam>));
}

using aliasing_source_types = ::testing::Types<
    arene::base::mdspan<float const, arene::base::extents<std::size_t, 3, 4>>,        // static extents
    arene::base::mdspan<std::uint32_t const, arene::base::dextents<std::size_t, 2>>,  // dynamic extents
    arene::base::mdspan<int const, arene::base::extents<std::size_t, 2, arene::base::dynamic_extent>>,  // mixed extents
    arene::base::mdspan<std::int16_t const, arene::base::extents<std::size_t, 4, 2>>>;  // static extents, wider matrix

template <class Matrix>
class RowColumnViewAliasingTest : public ::testing::Test {};

TYPED_TEST_SUITE(RowColumnViewAliasingTest, aliasing_source_types, );

/// @test each slice of @c row_view aliases @c submdspan(matrix, i, full_extent)
CONSTEXPR_TYPED_TEST(RowColumnViewAliasingTest, RowsAliasSubmdspan) {
  auto const matrix = make_test_mdspan<TypeParam>();

  auto row_index = typename TypeParam::index_type{0};
  for (auto const row : row_view(matrix)) {
    CONSTEXPR_ASSERT(equal_elements(row, submdspan(matrix, row_index, full_extent), equal_address{}));
    CONSTEXPR_ASSERT(
        equal_elements(row_view(matrix, row_index), submdspan(matrix, row_index, full_extent), equal_address{})
    );
    ++row_index;
  }
  CONSTEXPR_ASSERT(row_index == matrix.extent(0));
}

/// @test each slice of @c column_view aliases @c submdspan(matrix, full_extent, i)
CONSTEXPR_TYPED_TEST(RowColumnViewAliasingTest, ColumnsAliasSubmdspan) {
  auto const matrix = make_test_mdspan<TypeParam>();

  auto column_index = typename TypeParam::index_type{0};
  for (auto const column : column_view(matrix)) {
    CONSTEXPR_ASSERT(equal_elements(column, submdspan(matrix, full_extent, column_index), equal_address{}));
    CONSTEXPR_ASSERT(
        equal_elements(column_view(matrix, column_index), submdspan(matrix, full_extent, column_index), equal_address{})
    );
    ++column_index;
  }
  CONSTEXPR_ASSERT(column_index == matrix.extent(1));
}

}  // namespace
