// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/linalg/detail/mandates.hpp"
#include "arene/base/linalg/identity_matrix.hpp"
#include "arene/base/linalg/tests/test_helpers.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/type_list/filter.hpp"

// IWYU pragma: no_include "arene/base/mdspan/mdspan.hpp"

namespace {

namespace linalg = arene::base::linalg;
using arene::base::dynamic_extent;
using arene::base::extents;
using arene::base::linalg_testing::unqualified_linalg_matrix_types;
using arene::base::type_lists::filter_t;

/// @brief A set of non-square extents of the given type, if it's possible to construct them
/// @{
template <typename Extents>
constexpr Extents non_square_extents{};

template <typename IndexType, std::size_t Height>
constexpr extents<IndexType, Height, dynamic_extent> non_square_extents<extents<IndexType, Height, dynamic_extent>>{
    Height + 1U
};

template <typename IndexType, std::size_t Width>
constexpr extents<IndexType, dynamic_extent, Width> non_square_extents<extents<IndexType, dynamic_extent, Width>>{
    Width + 1U
};

template <typename IndexType>
constexpr extents<IndexType, dynamic_extent, dynamic_extent>
    non_square_extents<extents<IndexType, dynamic_extent, dynamic_extent>>{10U, 11U};
/// @}

template <typename T>
class IdentityMatrixDeathTest : public ::testing::Test {};

using possibly_square_matrix_types = filter_t<unqualified_linalg_matrix_types, linalg::possibly_square>;

TYPED_TEST_SUITE(IdentityMatrixDeathTest, possibly_square_matrix_types, );

/// @test @c identity_matrix crashes with a precondition violation when the passed extents are not actually square
TYPED_TEST(IdentityMatrixDeathTest, PreconditionViolationWhenNotActuallySquare) {
  using element_type = typename TypeParam::element_type;
  using extents_type = typename TypeParam::extents_type;

  extents_type const exts{non_square_extents<extents_type>};

  // Calling the function should crash if and only if the extents are actually non-square (which may be impossible).
  if (exts.extent(0) != exts.extent(1)) {
    ASSERT_DEATH(linalg::identity_matrix<element_type>(exts), "Precondition");
  } else {
    // Just for completeness, if the type is statically non-square we'll still make sure the function doesn't crash.
    std::ignore = linalg::identity_matrix<element_type>(exts);
  }
}

}  // namespace
