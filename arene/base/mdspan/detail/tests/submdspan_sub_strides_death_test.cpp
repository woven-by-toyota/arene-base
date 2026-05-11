// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/detail/submdspan_sub_strides.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"

namespace {

using arene::base::mdspan_detail::submdspan_sub_strides;

/// @test @c submdspan_sub_strides triggers a precondition violation when stride * multiplier overflows
TEST(SubmdspanSubStridesDeathTest, StrideMultiplierOverflow) {
  using src_extents = arene::base::extents<std::size_t, arene::base::dynamic_extent>;

  auto const huge = std::numeric_limits<std::size_t>::max() / 2U + 1U;

  // layout_stride with a large stride
  auto mapping =
      arene::base::layout_stride::mapping<src_extents>{src_extents{huge}, arene::base::array<std::size_t, 1>{huge}};

  // For coverage
  auto good_slice = arene::base::extent_slice<std::size_t, std::size_t, std::size_t>{0U, 1U, 1U};
  std::ignore = submdspan_sub_strides<std::size_t>(mapping, good_slice);

  // extent_slice with stride 2 so that huge * 2 overflows
  auto bad_slice = arene::base::extent_slice<std::size_t, std::size_t, std::size_t>{0U, huge, 2U};
  ASSERT_DEATH(std::ignore = submdspan_sub_strides<std::size_t>(mapping, bad_slice), "Precondition");
}

}  // namespace
