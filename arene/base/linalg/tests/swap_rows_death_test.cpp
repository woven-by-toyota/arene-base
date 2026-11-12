// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/swap_rows.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

// IWYU pragma: no_include "arene/base/stdlib_choice/forward.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/tuple.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/make_tuple.hpp"

namespace {
template <typename T>
class SwapRowsDeathTest : public ::testing::Test {};

// Just the in/out matrices from the test helpers header
using inout_matrices = ::testing::Types<
    arene::base::mdspan<std::int32_t, arene::base::extents<std::size_t, 3, 4>>,
    arene::base::mdspan<double, arene::base::extents<std::size_t, 1, arene::base::dynamic_extent>>,
    arene::base::mdspan<double, arene::base::dextents<std::size_t, 2>>,
    arene::base::mdspan<
        std::uint32_t,
        arene::base::extents<std::size_t, arene::base::dynamic_extent, 4>,
        arene::base::layout_left>,
    arene::base::mdspan<
        std::uint32_t,
        arene::base::extents<std::size_t, 6, arene::base::dynamic_extent>,
        arene::base::layout_stride>>;

TYPED_TEST_SUITE(SwapRowsDeathTest, inout_matrices, );

/// @test Calling @c swap_rows on an index that can't represent a row is a precondition violation
TYPED_TEST(SwapRowsDeathTest, OutOfBoundsIndexIsPreconditionViolation) {
  arene::base::linalg_testing::test_array<TypeParam> matrix_arr{};
  TypeParam const matrix{matrix_arr};

  typename TypeParam::index_type const in_bounds_idx{};
  typename TypeParam::index_type const out_of_bounds_idx{matrix.extent(0)};

  ASSERT_DEATH(arene::base::linalg::swap_rows(matrix, in_bounds_idx, out_of_bounds_idx), "Precondition");
  ASSERT_DEATH(arene::base::linalg::swap_rows(matrix, out_of_bounds_idx, in_bounds_idx), "Precondition");

  // If both row indices are out of bounds, there's a precondition violation whether they're equal or not.
  ASSERT_DEATH(arene::base::linalg::swap_rows(matrix, out_of_bounds_idx, out_of_bounds_idx), "Precondition");
  ASSERT_DEATH(arene::base::linalg::swap_rows(matrix, out_of_bounds_idx, out_of_bounds_idx + 1U), "Precondition");
}
}  // namespace
