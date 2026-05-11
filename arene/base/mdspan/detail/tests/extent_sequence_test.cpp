// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/extent_sequence.hpp"

#include <gtest/gtest.h>

#include "arene/base/detail/dynamic_extent.hpp"
#include "arene/base/stdlib_choice/equal_to.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test @c extent_sequence_equal returns @c true if sequences are equal, otherwise @c false
CONSTEXPR_TEST(MdspanDetailExtentSequence, ExtentSequenceEqual) {
  using arene::base::mdspan_detail::extent_sequence;
  using arene::base::mdspan_detail::extent_sequence_equal;

  CONSTEXPR_ASSERT(extent_sequence_equal(extent_sequence<>{}, extent_sequence<>{}, std::equal_to<>{}));
  CONSTEXPR_ASSERT(extent_sequence_equal(extent_sequence<1>{}, extent_sequence<1>{}, std::equal_to<>{}));
  CONSTEXPR_ASSERT(extent_sequence_equal(extent_sequence<1, 2>{}, extent_sequence<1, 2>{}, std::equal_to<>{}));

  CONSTEXPR_ASSERT(!extent_sequence_equal(extent_sequence<>{}, extent_sequence<1>{}, std::equal_to<>{}));
  CONSTEXPR_ASSERT(!extent_sequence_equal(extent_sequence<1>{}, extent_sequence<>{}, std::equal_to<>{}));
  CONSTEXPR_ASSERT(!extent_sequence_equal(extent_sequence<1, 2>{}, extent_sequence<2, 1>{}, std::equal_to<>{}));
}

/// @test @c left_static_extent_and_right_dynamic_extent returns @c true if the left extent is static and the right
/// extent is dynamic, otherwise @c false
CONSTEXPR_TEST(MdspanDetailExtentSequence, LeftStaticExtentAndRightDynamicExtent) {
  using arene::base::dynamic_extent;
  constexpr auto cmp = arene::base::mdspan_detail::left_static_extent_and_right_dynamic_extent{};

  CONSTEXPR_ASSERT(cmp(0U, dynamic_extent));
  CONSTEXPR_ASSERT(cmp(1U, dynamic_extent));
  CONSTEXPR_ASSERT(cmp(dynamic_extent - 1U, dynamic_extent));

  CONSTEXPR_ASSERT(!cmp(dynamic_extent, dynamic_extent));

  CONSTEXPR_ASSERT(!cmp(0U, 0U));
  CONSTEXPR_ASSERT(!cmp(0U, 1U));
  CONSTEXPR_ASSERT(!cmp(1U, 1U));

  CONSTEXPR_ASSERT(!cmp(dynamic_extent, 0U));
  CONSTEXPR_ASSERT(!cmp(dynamic_extent, 1U));
}
}  // namespace
