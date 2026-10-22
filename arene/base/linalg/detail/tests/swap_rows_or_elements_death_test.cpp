// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/linalg/detail/swap_rows_or_elements.hpp"
#include "arene/base/linalg/tests/test_array.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/mdspan.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace {

/// @test Given equal out-of-bounds vector indices, when swapping elements, then the precondition fails.
TEST(SwapRowsOrElementsDeathTest, EqualOutOfBoundsVectorIndicesAreAPreconditionViolation) {
  using vector_type = arene::base::mdspan<int, arene::base::extents<std::size_t, 3>>;
  arene::base::linalg_testing::test_array<vector_type> storage{};

  ASSERT_DEATH(arene::base::linalg::detail::swap_rows_or_elements(storage.mdspan(), 3U, 3U), "Precondition");
}

/// @test Given a vector index not representable in its index type, when swapping elements, then the precondition fails.
TEST(SwapRowsOrElementsDeathTest, UnrepresentableVectorIndexIsAPreconditionViolation) {
  using vector_type = arene::base::mdspan<int, arene::base::extents<std::uint8_t, 3>>;
  arene::base::linalg_testing::test_array<vector_type> storage{};
  auto const unrepresentable_index = std::size_t{300};

  ASSERT_DEATH(
      arene::base::linalg::detail::swap_rows_or_elements(storage.mdspan(), unrepresentable_index, std::size_t{0}),
      "is_representable_as"
  );
}

}  // namespace
