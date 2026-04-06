// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/is_valid_submdspan_slice_for.hpp"

#include <gtest/gtest.h>

#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::extent_slice;
using arene::base::full_extent;
using arene::base::mdspan_detail::is_valid_submdspan_slice_for;

/// @test @c is_valid_submdspan_slice_for returns @c true if the slice upper
/// bound is not greater than the extent value, otherwise it returns @c false
CONSTEXPR_TEST(MdspanDetailIsValidSubmdspanSliceForTest, IntegerSlice) {
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(0, 1));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(0, 2));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(1, 2));

  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(2, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(2, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(2, 0));

  // avoid constant folding in coverage builds
  std::ignore = is_valid_submdspan_slice_for(0, 1);
  std::ignore = is_valid_submdspan_slice_for(2, 1);
}

/// @test @c is_valid_submdspan_slice_for returns @c false for types convertible
/// to @c index_type if the value is negative
CONSTEXPR_TEST(MdspanDetailIsValidSubmdspanSliceForTest, NegativeIntegerSlice) {
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(-1, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(-2, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(-2, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(-2, 0));

  // avoid constant folding in coverage builds
  std::ignore = is_valid_submdspan_slice_for(-2, 1);
}

/// @test an @c extent_slice with a negative @c offset value is never valid
CONSTEXPR_TEST(MdspanDetailIsValidSubmdspanSliceForTest, ExtentSliceWithNegativeOffset) {
  using extent_slice_type = extent_slice<int, int, int>;

  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{-1, 0, 1}, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{-2, 0, 1}, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{-3, 0, 1}, 1));

  // avoid constant folding in coverage builds
  std::ignore = is_valid_submdspan_slice_for(extent_slice_type{-1, 0, 1}, 1);
}

/// @test an @c extent_slice with a negative @c extent value is never valid
CONSTEXPR_TEST(MdspanDetailIsValidSubmdspanSliceForTest, ExtentSliceWithNegativeExtent) {
  using extent_slice_type = extent_slice<int, int, int>;

  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, -1, 1}, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, -2, 1}, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, -3, 1}, 1));

  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, -1, 0}, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, -2, 0}, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, -3, 0}, 1));

  // avoid constant folding in coverage builds
  std::ignore = is_valid_submdspan_slice_for(extent_slice_type{0, -1, 1}, 1);
}

/// @test an @c extent_slice with a negative @c stride value is never valid
CONSTEXPR_TEST(MdspanDetailIsValidSubmdspanSliceForTest, ExtentSliceWithNegativeStride) {
  using extent_slice_type = extent_slice<int, int, int>;

  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, 0, -1}, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, 0, -2}, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, 0, -3}, 1));

  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, 1, -1}, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, 1, -2}, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, 1, -3}, 1));

  // avoid constant folding in coverage builds
  std::ignore = is_valid_submdspan_slice_for(extent_slice_type{0, 0, -1}, 1);
}

/// @test an @c extent_slice with a zero @c stride value is only valid if the
/// @c extent value is zreo
/// @note P3982R0 does not lift the requirements on the stride value being non-zero
CONSTEXPR_TEST(MdspanDetailIsValidSubmdspanSliceForTest, ExtentSliceWithZeroStride) {
  using extent_slice_type = extent_slice<int, int, int>;

  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{0, 0, 0}, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, 1, 0}, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, 2, 0}, 1));

  // avoid constant folding in coverage builds
  std::ignore = is_valid_submdspan_slice_for(extent_slice_type{0, 0, 0}, 1);
  std::ignore = is_valid_submdspan_slice_for(extent_slice_type{0, 1, 0}, 1);
}

/// @test @c is_valid_submdspan_slice_for returns @c true if the upper bound of
/// an @c extent_slice is not greater than the extent value and that @c
/// extent_slice has stride 1
CONSTEXPR_TEST(MdspanDetailIsValidSubmdspanSliceForTest, ExtentSliceWithStride1) {
  using extent_slice_type = extent_slice<int, int>;  // always a stride of 1

  // since slice.extent is 0 the slice upper bound will be slice.offset
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{0}, 1));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{0}, 2));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{1}, 2));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{2}, 2));

  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{3}, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{4}, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{5}, 2));

  // since slice.extent is 1 the slice upper bound will be slice.offset + 1
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{0, 1}, 1));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{0, 1}, 2));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{1, 1}, 2));

  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{2, 1}, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{3, 1}, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{4, 1}, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{5, 1}, 2));

  // since slice.extent is 2 the slice upper bound will be slice.offset + 1 + 1
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{0, 2}, 2));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{0, 2}, 3));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{1, 2}, 3));

  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, 2}, 1));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{2, 2}, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{3, 2}, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{4, 2}, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{5, 2}, 2));

  // avoid constant folding in coverage builds
  std::ignore = is_valid_submdspan_slice_for(extent_slice_type{0}, 2);
  std::ignore = is_valid_submdspan_slice_for(extent_slice_type{0, 1}, 2);
}

/// @test @c is_valid_submdspan_slice_for returns @c true if the upper bound of
/// an @c extent_slice is not greater than the extent value and that @c
/// extent_slice has stride greater than 1
CONSTEXPR_TEST(MdspanDetailIsValidSubmdspanSliceForTest, ExtentSliceWithStrideGreaterThan1) {
  using extent_slice_type = extent_slice<int, int, int>;

  // since slice.extent is 0 the slice upper bound will be slice.offset
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{0, 0, 2}, 1));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{0, 0, 2}, 2));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{1, 0, 2}, 2));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{2, 0, 2}, 2));

  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{3, 0, 2}, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{4, 0, 2}, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{5, 0, 2}, 2));

  // since slice.extent is 1 the slice upper bound will be slice.offset + 1
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{0, 1, 2}, 1));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{0, 1, 2}, 2));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{1, 1, 2}, 2));

  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{2, 1, 2}, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{3, 1, 2}, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{4, 1, 2}, 2));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{5, 1, 2}, 2));

  // since slice.extent is 2 the slice upper bound will be slice.offset + 1 + slice.stride
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{0, 2, 2}, 3));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{0, 2, 2}, 4));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{1, 2, 2}, 4));

  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{1, 2, 2}, 3));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{2, 2, 2}, 3));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{3, 2, 2}, 3));
}

/// @test @c is_valid_submdspan_slice_for returns @c false if the upper bound of
/// an @c extent_slice would "overflow" the index type
CONSTEXPR_TEST(MdspanDetailIsValidSubmdspanSliceForTest, ExtentSliceUpperBoundWouldOverflow) {
  using extent_slice_type = extent_slice<int, int, int>;

  constexpr auto huge = std::numeric_limits<int>::max();

  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{huge, 0, 1}, huge));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(extent_slice_type{(huge - 1), 1, 1}, huge));

  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{huge, 1, 1}, huge));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, 3, huge}, huge));
  CONSTEXPR_ASSERT(!is_valid_submdspan_slice_for(extent_slice_type{0, huge, huge}, huge));
}

/// @test @c is_valid_submdspan_slice_for is always @c true for a @c full_extent_t slice
CONSTEXPR_TEST(MdspanDetailIsValidSubmdspanSliceForTest, FullExtent) {
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(full_extent, 0));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(full_extent, 1));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(full_extent, 2));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(full_extent, 3));
  CONSTEXPR_ASSERT(is_valid_submdspan_slice_for(full_extent, 4));

  // avoid constant folding in coverage builds
  std::ignore = is_valid_submdspan_slice_for(full_extent, 4);
}

}  // namespace
