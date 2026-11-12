// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/detail/swap_rows_or_elements.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/linalg/detail/tests/proxy_accessor.hpp"
#include "arene/base/linalg/object_traits.hpp"
#include "arene/base/linalg/swap_rows.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"  // IWYU pragma: keep
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace {

using arene::base::array;
using arene::base::extents;
using arene::base::mdspan;
using arene::base::linalg::swap_rows;
using arene::base::linalg::detail::swap_rows_or_elements;
using arene::base::linalg_testing::integer_reference_proxy;
using arene::base::linalg_testing::proxy_accessor;
using arene::base::linalg_testing::test_array;

/// @test Given unsupported objects or an index with a throwing conversion, when checking the helper, then invocation
/// is disabled.
TEST(SwapRowsOrElementsTest, ConstraintsRejectUnsupportedArguments) {
  using swap_rows_or_elements_fn = decltype(swap_rows_or_elements);
  using vector_type = mdspan<int, extents<int, 3>>;
  using matrix_type = mdspan<int, extents<std::size_t, 3, 2>>;
  using scalar_type = mdspan<int, extents<int>>;
  using throwing_index = ::testing::configurable_value<int>;

  EXPECT_TRUE((arene::base::is_invocable_v<swap_rows_or_elements_fn, vector_type, int, int>));
  EXPECT_TRUE((arene::base::is_invocable_v<swap_rows_or_elements_fn, matrix_type, int, int>));
  EXPECT_FALSE((arene::base::is_invocable_v<swap_rows_or_elements_fn, scalar_type, int, int>));
  EXPECT_FALSE((arene::base::is_invocable_v<swap_rows_or_elements_fn, vector_type, throwing_index, int>));
  EXPECT_FALSE((arene::base::is_invocable_v<swap_rows_or_elements_fn, vector_type, int, throwing_index>));
}

/// @test Given matrices with nonthrowing and throwing swaps, when checking noexcept, then the helper matches
/// @c swap_rows.
CONSTEXPR_TEST(SwapRowsOrElementsTest, MatrixNoexceptMatchesSwapRows) {
  using nonthrowing_matrix = mdspan<::testing::configurable_value<int>, extents<std::size_t, 3, 2>>;
  using throwing_matrix =
      mdspan<::testing::configurable_value<int, ::testing::throws_on::swap>, extents<std::size_t, 3, 2>>;

  CONSTEXPR_ASSERT((arene::base::is_nothrow_invocable_v<decltype(swap_rows), nonthrowing_matrix, int, int>));
  CONSTEXPR_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<decltype(swap_rows_or_elements), nonthrowing_matrix, int, int>),
      (arene::base::is_nothrow_invocable_v<decltype(swap_rows), nonthrowing_matrix, int, int>)
  );
  CONSTEXPR_ASSERT_FALSE((arene::base::is_nothrow_invocable_v<decltype(swap_rows), throwing_matrix, int, int>));
  CONSTEXPR_ASSERT_EQ(
      (arene::base::is_nothrow_invocable_v<decltype(swap_rows_or_elements), throwing_matrix, int, int>),
      (arene::base::is_nothrow_invocable_v<decltype(swap_rows), throwing_matrix, int, int>)
  );
}

/// @test A vector whose accessor returns a proxy by value is rejected even when the proxy supports by-value swap.
TEST(SwapRowsOrElementsTest, RejectsProxyReferences) {
  using proxy_vector_type = mdspan<int, extents<std::size_t, 3>, arene::base::layout_right, proxy_accessor>;
  auto values = array<int, 2>{{1, 2}};
  swap(integer_reference_proxy{values[0]}, integer_reference_proxy{values[1]});

  EXPECT_EQ(values, (array<int, 2>{{2, 1}}));
  STATIC_ASSERT_TRUE(arene::base::linalg::is_inout_vector_v<proxy_vector_type>);
  STATIC_ASSERT_TRUE((arene::base::is_swappable_with_v<proxy_vector_type::reference, proxy_vector_type::reference>));
  STATIC_ASSERT_FALSE((arene::base::is_invocable_v<decltype(swap_rows_or_elements), proxy_vector_type, int, int>));
}

/// @test Given a vector, when two slices are swapped, then the corresponding elements exchange positions.
CONSTEXPR_TEST(SwapRowsOrElementsTest, SwapsVectorElements) {
  using vector_type = mdspan<int, extents<std::uint8_t, 3>>;
  test_array<vector_type> storage{array<int, 3>{{1, 2, 3}}};

  swap_rows_or_elements(storage.mdspan(), std::integral_constant<int, 0>{}, std::size_t{2});

  CONSTEXPR_ASSERT_EQ(storage.buffer(), (array<int, 3>{{3, 2, 1}}));
}

/// @test Given a vector, when an element is swapped with itself, then the vector remains unchanged.
CONSTEXPR_TEST(SwapRowsOrElementsTest, EqualVectorIndicesLeaveVectorUnchanged) {
  using vector_type = mdspan<int, extents<std::uint8_t, 3>>;
  test_array<vector_type> storage{array<int, 3>{{1, 2, 3}}};

  swap_rows_or_elements(storage.mdspan(), std::integral_constant<int, 0>{}, std::size_t{0});

  CONSTEXPR_ASSERT_EQ(storage.buffer(), (array<int, 3>{{1, 2, 3}}));
}

/// @test Given a matrix, when two slices are swapped, then the corresponding rows exchange positions.
CONSTEXPR_TEST(SwapRowsOrElementsTest, SwapsMatrixRows) {
  using matrix_type = mdspan<int, extents<std::size_t, 3, 2>>;
  test_array<matrix_type> storage{array<int, 6>{{1, 2, 3, 4, 5, 6}}};

  swap_rows_or_elements(storage.mdspan(), 0U, 2U);

  CONSTEXPR_ASSERT_EQ(storage.buffer(), (array<int, 6>{{5, 6, 3, 4, 1, 2}}));
}

}  // namespace
