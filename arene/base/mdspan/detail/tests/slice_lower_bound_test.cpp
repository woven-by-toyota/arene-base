// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/slice_lower_bound.hpp"

#include <gtest/gtest.h>

#include "arene/base/mdspan/full_extent.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::extent_slice;
using arene::base::full_extent;
using arene::base::mdspan_detail::slice_lower_bound;

/// @test @c slice_lower_bound returns the index value for an integer slice
CONSTEXPR_TEST(MdspanDetailSliceLowerBoundTest, IntegerSlice) {
  auto const result = slice_lower_bound<std::size_t>(std::size_t{3});

  CONSTEXPR_ASSERT(result == 3);
  CONSTEXPR_ASSERT((std::is_same<decltype(result), std::size_t const>::value));
}

/// @test @c slice_lower_bound returns the index value for an integral-constant slice
CONSTEXPR_TEST(MdspanDetailSliceLowerBoundTest, IntegralConstant) {
  auto const result = slice_lower_bound<std::size_t>(std::integral_constant<std::size_t, 0>{});

  CONSTEXPR_ASSERT(result == 0);
}

/// @test @c slice_lower_bound returns the offset from an extent_slice
CONSTEXPR_TEST(MdspanDetailSliceLowerBoundTest, ExtentSlice) {
  auto slice = extent_slice<std::size_t>{std::size_t{5}, std::size_t{10}};
  auto const result = slice_lower_bound<std::size_t>(slice);

  CONSTEXPR_ASSERT(result == 5);
}

/// @test @c slice_lower_bound returns the offset from an extent_slice with integral-constant offset
CONSTEXPR_TEST(MdspanDetailSliceLowerBoundTest, IntegralConstantExtentSlice) {
  auto slice = extent_slice<std::integral_constant<std::size_t, 5>, std::size_t>{{}, std::size_t{10}};
  auto const result = slice_lower_bound<std::size_t>(slice);

  CONSTEXPR_ASSERT(result == 5);
}

/// @test @c slice_lower_bound returns 0 for full_extent_t
CONSTEXPR_TEST(MdspanDetailSliceLowerBoundTest, FullExtent) {
  auto const result = slice_lower_bound<std::size_t>(full_extent);

  CONSTEXPR_ASSERT(result == 0);
}

}  // namespace
