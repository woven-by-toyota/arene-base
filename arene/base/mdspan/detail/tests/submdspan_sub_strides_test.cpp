// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/submdspan_sub_strides.hpp"

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/layout.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {

using arene::base::array;
using arene::base::dextents;
using arene::base::extent_slice;
using arene::base::extents;
using arene::base::full_extent;
using arene::base::full_extent_t;
using arene::base::is_invocable_v;
using arene::base::layout_right;
using arene::base::layout_stride;
using arene::base::mdspan_detail::submdspan_sub_strides;

/// @test @c submdspan_sub_strides preserves both strides with full_extent slices
CONSTEXPR_TEST(SubmdspanSubStridesTest, FullExtentSlices) {
  using src_extents = extents<std::size_t, 30, 20>;
  auto mapping = layout_right::mapping<src_extents>{};
  auto result = submdspan_sub_strides<std::size_t>(mapping, full_extent, full_extent);

  CONSTEXPR_ASSERT(result[0] == mapping.stride(0));
  CONSTEXPR_ASSERT(result[1] == mapping.stride(1));
}

/// @test @c submdspan_sub_strides with integral collapse of the first dimension
CONSTEXPR_TEST(SubmdspanSubStridesTest, IntegralCollapseFirstDim) {
  using src_extents = extents<std::size_t, 30, 20>;
  auto mapping = layout_right::mapping<src_extents>{};
  auto result = submdspan_sub_strides<std::size_t>(mapping, std::size_t{0}, full_extent);

  CONSTEXPR_ASSERT(result[0] == mapping.stride(1));
}

/// @test @c submdspan_sub_strides with integral collapse of the second dimension
CONSTEXPR_TEST(SubmdspanSubStridesTest, IntegralCollapseSecondDim) {
  using src_extents = extents<std::size_t, 30, 20>;
  auto mapping = layout_right::mapping<src_extents>{};
  auto result = submdspan_sub_strides<std::size_t>(mapping, full_extent, std::size_t{0});

  CONSTEXPR_ASSERT(result[0] == mapping.stride(0));
}

/// @test @c submdspan_sub_strides with extent_slice stride variations
CONSTEXPR_TEST(SubmdspanSubStridesTest, ExtentSliceStrideVariations) {
  using src_extents = extents<std::size_t, 10>;
  auto mapping = layout_right::mapping<src_extents>{};

  // extent > 1 with stride < extent: multiplies stride into result
  auto slice_lt = extent_slice<std::size_t, std::size_t, std::size_t>{0U, 5U, 2U};
  auto result_lt = submdspan_sub_strides<std::size_t>(mapping, slice_lt);
  CONSTEXPR_ASSERT(result_lt[0] == mapping.stride(0) * slice_lt.stride);

  // extent > 1 with stride == extent: multiplies stride into result
  auto slice_eq = extent_slice<std::size_t, std::size_t, std::size_t>{0U, 5U, 5U};
  auto result_eq = submdspan_sub_strides<std::size_t>(mapping, slice_eq);
  CONSTEXPR_ASSERT(result_eq[0] == mapping.stride(0) * slice_eq.stride);

  // extent > 1 with stride > extent: multiplies stride into result
  auto slice_gt = extent_slice<std::size_t, std::size_t, std::size_t>{0U, 3U, 10U};
  auto result_gt = submdspan_sub_strides<std::size_t>(mapping, slice_gt);
  CONSTEXPR_ASSERT(result_gt[0] == mapping.stride(0) * slice_gt.stride);

  // extent == 1: no multiply
  auto result_one =
      submdspan_sub_strides<std::size_t>(mapping, extent_slice<std::size_t, std::size_t, std::size_t>{0U, 1U, 5U});
  CONSTEXPR_ASSERT(result_one[0] == mapping.stride(0));

  // extent == 0: no multiply
  auto result_zero =
      submdspan_sub_strides<std::size_t>(mapping, extent_slice<std::size_t, std::size_t, std::size_t>{0U, 0U, 5U});
  CONSTEXPR_ASSERT(result_zero[0] == mapping.stride(0));
}

/// @test @c submdspan_sub_strides with extent_slice using default stride (integral_constant<size_t, 1>)
CONSTEXPR_TEST(SubmdspanSubStridesTest, ExtentSliceDefaultStride) {
  using src_extents = extents<std::size_t, 10>;
  auto mapping = layout_right::mapping<src_extents>{};
  auto result = submdspan_sub_strides<std::size_t>(mapping, extent_slice<std::size_t, std::size_t>{0U, 5U});

  CONSTEXPR_ASSERT(result[0] == mapping.stride(0));
}

/// @test @c submdspan_sub_strides with mixed slices in 3D
CONSTEXPR_TEST(SubmdspanSubStridesTest, MixedSlices3D) {
  using src_extents = extents<std::size_t, 4, 6, 8>;
  auto mapping = layout_right::mapping<src_extents>{};
  auto slice = extent_slice<std::size_t, std::size_t, std::size_t>{0U, 4U, 2U};
  auto result = submdspan_sub_strides<std::size_t>(mapping, std::size_t{0}, full_extent, slice);

  CONSTEXPR_ASSERT(result[0] == mapping.stride(1));
  CONSTEXPR_ASSERT(result[1] == mapping.stride(2) * slice.stride);
}

/// @test @c submdspan_sub_strides returns an empty array when all dimensions are collapsed
CONSTEXPR_TEST(SubmdspanSubStridesTest, AllCollapsed) {
  using src_extents = extents<std::size_t, 3, 4>;
  auto mapping = layout_right::mapping<src_extents>{};

  // Try to avoid constant folding
  std::ignore = submdspan_sub_strides<std::size_t>(mapping, std::size_t{0}, std::size_t{0});

  auto result = submdspan_sub_strides<std::size_t>(mapping, std::size_t{0}, std::size_t{0});

  CONSTEXPR_ASSERT(result.size() == 0U);
}

/// @test @c submdspan_sub_strides works with dynamic extents
CONSTEXPR_TEST(SubmdspanSubStridesTest, DynamicExtents) {
  using dyn_extents = extents<std::size_t, arene::base::dynamic_extent, arene::base::dynamic_extent>;
  auto mapping = layout_right::mapping<dyn_extents>{dyn_extents{3U, 4U}};
  auto result = submdspan_sub_strides<std::size_t>(mapping, std::size_t{0}, full_extent);

  CONSTEXPR_ASSERT(result[0] == mapping.stride(1));
}

/// @test @c submdspan_sub_strides preserves custom strides from a @c layout_stride mapping
CONSTEXPR_TEST(SubmdspanSubStridesLayoutStrideTest, CustomStrides) {
  using src_extents = extents<std::size_t, 4, 6>;
  // strides: dim0 has stride 10, dim1 has stride 1 (column-major-like with padding)
  auto mapping = layout_stride::mapping<src_extents>{src_extents{}, array<std::size_t, 2>{10U, 1U}};
  auto result = submdspan_sub_strides<std::size_t>(mapping, full_extent, full_extent);

  CONSTEXPR_ASSERT(result[0] == mapping.stride(0));
  CONSTEXPR_ASSERT(result[1] == mapping.stride(1));
}

/// @test @c submdspan_sub_strides with custom strides and integral collapse
CONSTEXPR_TEST(SubmdspanSubStridesLayoutStrideTest, CustomStridesCollapse) {
  using src_extents = extents<std::size_t, 4, 6, 8>;
  // strides: dim0=48, dim1=8, dim2=1 (row-major)
  auto mapping = layout_stride::mapping<src_extents>{src_extents{}, array<std::size_t, 3>{48U, 8U, 1U}};
  auto result = submdspan_sub_strides<std::size_t>(mapping, std::size_t{0}, full_extent, std::size_t{0});

  CONSTEXPR_ASSERT(result[0] == mapping.stride(1));
}

/// @test @c submdspan_sub_strides multiplies custom stride by extent_slice stride
CONSTEXPR_TEST(SubmdspanSubStridesLayoutStrideTest, CustomStridesWithExtentSlice) {
  using src_extents = extents<std::size_t, 10>;
  // custom stride of 3 (e.g. from a previous submdspan with padding)
  auto mapping = layout_stride::mapping<src_extents>{src_extents{}, array<std::size_t, 1>{3U}};
  auto slice = extent_slice<std::size_t, std::size_t, std::size_t>{0U, 5U, 2U};
  auto result = submdspan_sub_strides<std::size_t>(mapping, slice);

  CONSTEXPR_ASSERT(result[0] == mapping.stride(0) * slice.stride);
}

/// @test @c submdspan_sub_strides is invocable when the number of slices matches the mapping rank
CONSTEXPR_TEST(SubmdspanSubStridesInvocabilityTest, InvocableWithMatchingSliceCount) {
  using mapping_0d = layout_right::mapping<dextents<std::size_t, 0>>;
  using mapping_1d = layout_right::mapping<dextents<std::size_t, 1>>;
  using mapping_2d = layout_right::mapping<dextents<std::size_t, 2>>;
  using mapping_3d = layout_right::mapping<dextents<std::size_t, 3>>;

  CONSTEXPR_ASSERT(is_invocable_v<  //
                   decltype(submdspan_sub_strides<std::size_t>),
                   mapping_0d>);
  CONSTEXPR_ASSERT(is_invocable_v<  //
                   decltype(submdspan_sub_strides<std::size_t>),
                   mapping_1d,
                   full_extent_t>);
  CONSTEXPR_ASSERT(is_invocable_v<  //
                   decltype(submdspan_sub_strides<std::size_t>),
                   mapping_2d,
                   full_extent_t,
                   std::size_t>);
  CONSTEXPR_ASSERT(is_invocable_v<  //
                   decltype(submdspan_sub_strides<std::size_t>),
                   mapping_3d,
                   full_extent_t,
                   full_extent_t,
                   full_extent_t>);
}

/// @test @c submdspan_sub_strides is not invocable when the number of slices does not match the mapping rank
CONSTEXPR_TEST(SubmdspanSubStridesInvocabilityTest, NotInvocableWithMismatchedSliceCount) {
  using mapping_1d = layout_right::mapping<dextents<std::size_t, 1>>;
  using mapping_2d = layout_right::mapping<dextents<std::size_t, 2>>;

  CONSTEXPR_ASSERT(!is_invocable_v<  //
                   decltype(submdspan_sub_strides<std::size_t>),
                   mapping_1d>);
  CONSTEXPR_ASSERT(!is_invocable_v<  //
                   decltype(submdspan_sub_strides<std::size_t>),
                   mapping_1d,
                   full_extent_t,
                   full_extent_t>);
  CONSTEXPR_ASSERT(!is_invocable_v<  //
                   decltype(submdspan_sub_strides<std::size_t>),
                   mapping_2d,
                   full_extent_t>);
  CONSTEXPR_ASSERT(!is_invocable_v<  //
                   decltype(submdspan_sub_strides<std::size_t>),
                   mapping_2d,
                   full_extent_t,
                   full_extent_t,
                   full_extent_t>);
}

}  // namespace
