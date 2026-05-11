// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/reverse_extents.hpp"

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/mdspan/extents.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"

namespace {

using arene::base::mdspan_detail::reverse_extents_t;

/// @test reversing a rank-0 extents yields the same type
TEST(ReverseExtents, Rank0IsIdentity) {
  using original = arene::base::extents<std::int32_t>;
  ::testing::StaticAssertTypeEq<original, reverse_extents_t<original>>();
}

/// @test reversing a rank-1 extents yields the same type
TEST(ReverseExtents, Rank1IsIdentity) {
  using original = arene::base::extents<std::int32_t, 3>;
  ::testing::StaticAssertTypeEq<original, reverse_extents_t<original>>();
}

/// @test reversing a rank-2 extents swaps the extent values
TEST(ReverseExtents, Rank2SwapsExtents) {
  using original = arene::base::extents<std::int32_t, 3, 5>;
  using expected = arene::base::extents<std::int32_t, 5, 3>;
  ::testing::StaticAssertTypeEq<expected, reverse_extents_t<original>>();
}

/// @test reversing a rank-3 extents reverses the extent values
TEST(ReverseExtents, Rank3ReversesExtents) {
  using original = arene::base::extents<std::int32_t, 3, 4, 5>;
  using expected = arene::base::extents<std::int32_t, 5, 4, 3>;
  ::testing::StaticAssertTypeEq<expected, reverse_extents_t<original>>();
}

/// @test reversing preserves dynamic_extent markers in reversed positions
TEST(ReverseExtents, PreservesDynamicExtent) {
  using original = arene::base::extents<std::int32_t, arene::base::dynamic_extent, 4, 5>;
  using expected = arene::base::extents<std::int32_t, 5, 4, arene::base::dynamic_extent>;
  ::testing::StaticAssertTypeEq<expected, reverse_extents_t<original>>();
}

/// @test reversing twice yields the original type
TEST(ReverseExtents, ReversingTwiceIsIdentity) {
  using original = arene::base::extents<std::int32_t, 3, arene::base::dynamic_extent, 5>;
  ::testing::StaticAssertTypeEq<original, reverse_extents_t<reverse_extents_t<original>>>();
}

/// @test reversing preserves the index type
TEST(ReverseExtents, PreservesIndexType) {
  using original = arene::base::extents<std::size_t, 3, 4>;
  using expected = arene::base::extents<std::size_t, 4, 3>;
  ::testing::StaticAssertTypeEq<expected, reverse_extents_t<original>>();
}

}  // namespace
