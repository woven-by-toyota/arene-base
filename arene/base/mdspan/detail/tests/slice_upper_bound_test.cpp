// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/slice_upper_bound.hpp"

#include <gtest/gtest.h>

#include "arene/base/contracts/contract.hpp"
#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::extent_slice;
using arene::base::full_extent;
using arene::base::mdspan_detail::slice_upper_bound;

template <class IndexType, class SliceSpecifier>
constexpr auto unwrapped_slice_upper_bound(IndexType extent, SliceSpecifier slice) {
  auto const result = slice_upper_bound(extent, slice);
  ARENE_PRECONDITION(!result.overflowed);
  return result.value;
}

// test value representing @c e.extent(k), i.e. the value of @c extents at rank
// @c k
constexpr auto e_k = 42;

/// @test the result of @c slice_upper_bound is not representable as index type
/// if the slice is equal to the maximum representable value of index type
CONSTEXPR_TEST(MdspanDetailSliceUpperBoundTest, IntegerNotRepresentable) {
  auto const result = slice_upper_bound(std::int8_t{}, std::int8_t{127});
  CONSTEXPR_ASSERT(result.overflowed);
}

/// @test @c slice_upper_bound returns the successor value for an slice convertible to @c index_type
CONSTEXPR_TEST(MdspanDetailSliceUpperBoundTest, IntegerSlice) {
  CONSTEXPR_ASSERT(1 == unwrapped_slice_upper_bound(e_k, 0));
  CONSTEXPR_ASSERT(2 == unwrapped_slice_upper_bound(e_k, 1));
  CONSTEXPR_ASSERT(3 == unwrapped_slice_upper_bound(e_k, 2));
  CONSTEXPR_ASSERT(4 == unwrapped_slice_upper_bound(e_k, 3));

  // avoid constant folding in coverage builds
  std::ignore = unwrapped_slice_upper_bound(e_k, 0);
}

/// @test the result of @c slice_upper_bound is not representable as index type
/// if the calculation of the upper bound "overflows" the index type
CONSTEXPR_TEST(MdspanDetailSliceUpperBoundTest, ExtentSliceNotRepresentable) {
  using extent_slice_type = extent_slice<std::int8_t, std::int8_t, std::int8_t>;

  {
    auto const result = slice_upper_bound(std::int8_t{}, extent_slice_type{127, 1, 1});
    CONSTEXPR_ASSERT(result.overflowed);
  }

  {
    auto const result = slice_upper_bound(std::int8_t{}, extent_slice_type{64, 2, 64});
    CONSTEXPR_ASSERT(result.overflowed);
  }

  {
    auto const result = slice_upper_bound(std::int8_t{}, extent_slice_type{0, 3, 64});
    CONSTEXPR_ASSERT(result.overflowed);
  }

  {
    auto const result = slice_upper_bound(std::int8_t{}, extent_slice_type{2, 3, 63});
    CONSTEXPR_ASSERT(result.overflowed);
  }
}

/// @test @c slice_upper_bound returns the offset from an @c extent_slice with
/// an extent value of zero
CONSTEXPR_TEST(MdspanDetailSliceUpperBoundTest, ExtentSliceWithZeroExtent) {
  auto const slice1 = extent_slice<int>{5};
  CONSTEXPR_ASSERT(slice1.offset == unwrapped_slice_upper_bound(e_k, slice1));

  auto const slice2 = extent_slice<std::integral_constant<int, 5>, std::integral_constant<int, 0>>{};
  CONSTEXPR_ASSERT(slice2.offset == unwrapped_slice_upper_bound(e_k, slice2));

  // avoid constant folding in coverage builds
  std::ignore = unwrapped_slice_upper_bound(e_k, slice1);
  std::ignore = unwrapped_slice_upper_bound(e_k, slice2);
}

/// @test @c unwrapped_slice_upper_bound returns the offset plus the distance spanned by
/// the extent and stride from an @c extent_slice with if the extent value is
/// not zero
CONSTEXPR_TEST(MdspanDetailSliceUpperBoundTest, ExtentSliceWithNonZeroExtent) {
  auto const slice = extent_slice<  //
      std::integral_constant<int, 5>,
      std::integral_constant<int, 3>,
      std::integral_constant<int, 2>>{};

  CONSTEXPR_ASSERT((5 + 1 + (3 - 1) * 2) == unwrapped_slice_upper_bound(e_k, slice));
  // avoid constant folding in coverage builds
  std::ignore = unwrapped_slice_upper_bound(e_k, slice);

  using extent_slice_type = extent_slice<int, int, int>;

  CONSTEXPR_ASSERT((5 + 1 + (3 - 1) * 2) == unwrapped_slice_upper_bound(e_k, extent_slice_type{5, 3, 2}));
  CONSTEXPR_ASSERT((0 + 1 + (1 - 1) * 1) == unwrapped_slice_upper_bound(e_k, extent_slice_type{0, 1, 1}));
  CONSTEXPR_ASSERT((0 + 1 + (1 - 1) * 2) == unwrapped_slice_upper_bound(e_k, extent_slice_type{0, 1, 2}));
  CONSTEXPR_ASSERT((0 + 1 + (2 - 1) * 1) == unwrapped_slice_upper_bound(e_k, extent_slice_type{0, 2, 1}));
  CONSTEXPR_ASSERT((0 + 1 + (2 - 1) * 2) == unwrapped_slice_upper_bound(e_k, extent_slice_type{0, 2, 2}));
  CONSTEXPR_ASSERT((1 + 1 + (2 - 1) * 2) == unwrapped_slice_upper_bound(e_k, extent_slice_type{1, 2, 2}));
  CONSTEXPR_ASSERT((2 + 1 + (2 - 1) * 2) == unwrapped_slice_upper_bound(e_k, extent_slice_type{2, 2, 2}));

  // avoid constant folding in coverage builds
  std::ignore = unwrapped_slice_upper_bound(e_k, extent_slice_type{5, 3, 2});

  CONSTEXPR_ASSERT(
      (2 + 1 + (2 - 1) * 2) ==
      unwrapped_slice_upper_bound(std::size_t{e_k}, extent_slice<std::size_t, std::size_t, std::size_t>{2, 2, 2})
  );

  std::ignore =
      unwrapped_slice_upper_bound(std::size_t{e_k}, extent_slice<std::size_t, std::size_t, std::size_t>{2, 2, 2});
}

/// @test @c unwrapped_slice_upper_bound returns @c extent for @c full_extent_t
CONSTEXPR_TEST(MdspanDetailSliceUpperBoundTest, FullExtent) {
  auto const result = unwrapped_slice_upper_bound(e_k, full_extent);

  CONSTEXPR_ASSERT(result == e_k);

  // avoid constant folding in coverage builds
  std::ignore = unwrapped_slice_upper_bound(e_k, full_extent);
}

}  // namespace
