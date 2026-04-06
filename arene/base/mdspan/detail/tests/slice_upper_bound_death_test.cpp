// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/mdspan/detail/slice_upper_bound.hpp"
#include "arene/base/mdspan/slice.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace {

using arene::base::extent_slice;

// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
auto const slice_upper_bound = [](auto slice) {
  constexpr auto e_k = std::int8_t{};  // the value here doesn't matter
  return arene::base::mdspan_detail::slice_upper_bound(e_k, slice);
};

using extent_slice_type = extent_slice<std::int8_t, std::int8_t, std::int8_t>;

/// @test @c slice_upper_bound triggers a precondition violation for a slice
/// convertible to @c index_type when the value is negative
TEST(MdspanDetailSliceUpperBoundDeathTest, IntegerIsNegative) {
  ASSERT_DEATH(slice_upper_bound(std::int8_t{-1}), "Precondition");
}

/// @test @c slice_upper_bound triggers a precondition violation for an @c
/// extent_slice with negative values
TEST(MdspanDetailSliceUpperBoundDeathTest, ExtentSliceWithNegativeValues) {
  ASSERT_DEATH(slice_upper_bound(extent_slice_type{-1, 0, 0}), "Precondition");
  ASSERT_DEATH(slice_upper_bound(extent_slice_type{0, -1, 0}), "Precondition");
  ASSERT_DEATH(slice_upper_bound(extent_slice_type{0, 0, -1}), "Precondition");
}

}  // namespace
