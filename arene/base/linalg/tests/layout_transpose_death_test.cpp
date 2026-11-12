// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/linalg/layout_transpose.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/tests/test_layout_types.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"

namespace {

using arene::base::extents;
using arene::base::layout_right;
using arene::base::linalg::layout_transpose;

constexpr std::uint32_t nested_rows{3U};
constexpr std::uint32_t nested_cols{5U};
using nested_extents = extents<std::uint32_t, nested_rows, nested_cols>;
using transposed_extents = extents<std::uint32_t, nested_cols, nested_rows>;
// The rank, used as an out-of-range dimension index for stride().
constexpr std::uint32_t rank{2U};

// A non-strided rank-2 layout, so layout_transpose::stride() must reject it at runtime.
using non_strided_layout = test::user_defined_layout</*Strided=*/false, /*Exhaustive=*/true, /*Unique=*/true>;

/// @test Calling `stride` with an out-of-range dimension index violates a precondition
TEST(LayoutTransposeDeathTest, StrideOutOfRangeIsPreconditionViolation) {
  layout_transpose<layout_right>::mapping<transposed_extents> const layout{layout_right::mapping<nested_extents>{}};
  ASSERT_DEATH(std::ignore = layout.stride(rank), "Precondition");
}

/// @test Calling `stride` on a non-strided nested mapping violates a precondition
TEST(LayoutTransposeDeathTest, StrideOnNonStridedIsPreconditionViolation) {
  layout_transpose<non_strided_layout>::mapping<transposed_extents> const layout{
      non_strided_layout::mapping<nested_extents>{}
  };
  ASSERT_DEATH(std::ignore = layout.stride(0U), "Precondition");
}

}  // namespace
