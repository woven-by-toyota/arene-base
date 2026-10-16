// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/matrix_observers.hpp"

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using arene::base::dynamic_extent;
using arene::base::extents;
using arene::base::is_invocable_v;
using arene::base::is_nothrow_invocable_v;
using arene::base::mdspan;
using arene::base::linalg::column_count;
using arene::base::linalg::is_square;
using arene::base::linalg::row_count;
using arene::base::linalg::static_column_count;
using arene::base::linalg::static_column_count_v;
using arene::base::linalg::static_row_count;
using arene::base::linalg::static_row_count_v;
using arene::base::linalg_testing::category;
using arene::base::linalg_testing::dynamic_test_array;
using arene::base::linalg_testing::test_array;

template <typename T>
class LinalgMatrixObserversTypedTest : public ::testing::Test {};

TYPED_TEST_SUITE(LinalgMatrixObserversTypedTest, arene::base::linalg_testing::unqualified_mdspan_types, );

/// @test @c row_count and @c column_count are invocable, and @c noexcept, for exactly the in-matrices
TYPED_TEST(LinalgMatrixObserversTypedTest, InvocableOnlyForMatrices) {
  constexpr bool is_matrix{
      (TypeParam::expectation == category::in_matrix) || (TypeParam::expectation == category::inout_matrix)
  };

  STATIC_ASSERT_EQ((is_invocable_v<decltype(row_count), typename TypeParam::type>), is_matrix);
  STATIC_ASSERT_EQ((is_invocable_v<decltype(column_count), typename TypeParam::type>), is_matrix);
  STATIC_ASSERT_EQ((is_nothrow_invocable_v<decltype(row_count), typename TypeParam::type>), is_matrix);
  STATIC_ASSERT_EQ((is_nothrow_invocable_v<decltype(column_count), typename TypeParam::type>), is_matrix);
}

/// @test @c is_square is invocable, and @c noexcept, for exactly the in-matrices
TYPED_TEST(LinalgMatrixObserversTypedTest, IsSquareInvocableOnlyForMatrices) {
  using matrix_type = typename TypeParam::type;
  constexpr bool is_matrix{
      (TypeParam::expectation == category::in_matrix) || (TypeParam::expectation == category::inout_matrix)
  };

  STATIC_ASSERT_EQ((is_invocable_v<decltype(is_square), matrix_type>), is_matrix);
  STATIC_ASSERT_EQ((is_nothrow_invocable_v<decltype(is_square), matrix_type>), is_matrix);
}

template <typename T>
class LinalgMatrixObserversMatrixTest : public ::testing::Test {};

TYPED_TEST_SUITE(LinalgMatrixObserversMatrixTest, arene::base::linalg_testing::unqualified_linalg_matrix_types, );

/// @test @c row_count returns @c extent(0) and @c column_count returns @c extent(1)
CONSTEXPR_TYPED_TEST(LinalgMatrixObserversMatrixTest, ReturnsCurrentExtents) {
  test_array<TypeParam> matrix_arr{};
  TypeParam const mat{matrix_arr};

  CONSTEXPR_ASSERT_EQ(row_count(mat), mat.extent(0));
  CONSTEXPR_ASSERT_EQ(column_count(mat), mat.extent(1));
}

/// @test @c is_square compares the current extents of every matrix
CONSTEXPR_TYPED_TEST(LinalgMatrixObserversMatrixTest, IsSquareMatchesCurrentExtents) {
  test_array<TypeParam> matrix_arr{};
  TypeParam const mat{matrix_arr};

  CONSTEXPR_ASSERT_EQ(is_square(mat), mat.extent(0) == mat.extent(1));
}

/// @test @c static_row_count_v and @c static_column_count_v report the extents type's static extent for each axis
TYPED_TEST(LinalgMatrixObserversMatrixTest, StaticExtentsMatchExtentsType) {
  using extents_type = typename TypeParam::extents_type;

  STATIC_ASSERT_EQ(static_row_count_v<TypeParam>, extents_type::static_extent(0));
  STATIC_ASSERT_EQ(static_column_count_v<TypeParam>, extents_type::static_extent(1));

  STATIC_ASSERT_EQ((static_row_count<TypeParam>::value), extents_type::static_extent(0));
  STATIC_ASSERT_EQ((static_column_count<TypeParam>::value), extents_type::static_extent(1));
}

/// @test @c row_count and @c column_count do not change the type returned by @c extent
TYPED_TEST(LinalgMatrixObserversMatrixTest, ReturnsExtentType) {
  using row_result = decltype(std::declval<TypeParam>().extent(0));
  using column_result = decltype(std::declval<TypeParam>().extent(1));

  STATIC_ASSERT_TRUE((std::is_same<decltype(row_count(std::declval<TypeParam>())), row_result>::value));
  STATIC_ASSERT_TRUE((std::is_same<decltype(column_count(std::declval<TypeParam>())), column_result>::value));
}

/// @test @c static_row_count_v and @c static_column_count_v report known static extents and @c dynamic_extent
TEST(LinalgMatrixObservers, StaticExtentsMatchKnownValues) {
  using row_static_t = mdspan<int, extents<std::size_t, 7, dynamic_extent>>;
  using column_static_t = mdspan<int, extents<std::size_t, dynamic_extent, 4>>;

  STATIC_ASSERT_EQ(static_row_count_v<row_static_t>, std::size_t{7});
  STATIC_ASSERT_EQ(static_column_count_v<row_static_t>, dynamic_extent);

  STATIC_ASSERT_EQ(static_row_count_v<column_static_t>, dynamic_extent);
  STATIC_ASSERT_EQ(static_column_count_v<column_static_t>, std::size_t{4});
}

/// @test @c static_row_count and @c static_column_count are @c std::integral_constant specializations
TEST(LinalgMatrixObservers, StaticExtentTraitsAreIntegralConstants) {
  using matrix_t = mdspan<int, extents<std::size_t, 3, 5>>;
  using expected_row_count = std::integral_constant<std::size_t, 3>;
  using expected_column_count = std::integral_constant<std::size_t, 5>;

  STATIC_ASSERT_TRUE((std::is_same<static_row_count<matrix_t>::type, expected_row_count>::value));
  STATIC_ASSERT_TRUE((std::is_same<static_column_count<matrix_t>::type, expected_column_count>::value));
}

/// @test @c static_row_count_v and @c static_column_count_v can be ODR-used, not only read in a constant expression
TEST(LinalgMatrixObservers, StaticExtentsAreOdrUsable) {
  using matrix_t = mdspan<int, extents<std::size_t, 3, 5>>;

  ASSERT_EQ(static_row_count_v<matrix_t>, std::size_t{3});
  ASSERT_EQ(static_column_count_v<matrix_t>, std::size_t{5});
}

/// @test @c is_square reports known static and runtime shapes
TEST(LinalgMatrixObservers, IsSquareReportsKnownShapes) {
  test_array<mdspan<int, extents<std::size_t, 4, 4>>> static_square_arr{};
  test_array<mdspan<int, extents<std::size_t, 3, 4>>> static_oblong_arr{};
  dynamic_test_array<int, std::size_t, 3, 3> dynamic_square_arr{};
  dynamic_test_array<int, std::size_t, 3, 4> dynamic_oblong_arr{};

  ASSERT_TRUE(is_square(static_square_arr.mdspan()));
  ASSERT_FALSE(is_square(static_oblong_arr.mdspan()));
  ASSERT_TRUE(is_square(dynamic_square_arr.mdspan()));
  ASSERT_FALSE(is_square(dynamic_oblong_arr.mdspan()));
}

}  // namespace
