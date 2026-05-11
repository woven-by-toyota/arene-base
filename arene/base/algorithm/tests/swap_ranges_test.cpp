// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/swap_ranges.hpp"

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "arene/base/algorithm/tests/test_iterators.hpp"
#include "arene/base/array.hpp"
#include "arene/base/compare.hpp"
#include "arene/base/detail/wrapped_iterator.hpp"

namespace {

/// @test `swap_range` of two empty vectors.
TEST(SwapRanges, SwapEmpty) {
  auto values = std::vector<int>();
  ASSERT_NO_FATAL_FAILURE(arene::base::swap_ranges(values.begin(), values.end(), values.begin()));
}

/// @test `swap_range` of a vector with two elements.
TEST(SwapRanges, SwapSubvectorOf1) {
  auto values = std::vector<int>{1, 0};
  arene::base::swap_ranges(values.begin(), values.begin() + 1, values.begin() + 1);
  ASSERT_EQ(values[0], 0);
  ASSERT_EQ(values[1], 1);
}

/// @test `swap_range` of a vector with four elements by two.
TEST(SwapRanges, SwapSubvectorOf2) {
  auto values = std::vector<int>{1, 2, 0, 0};
  arene::base::swap_ranges(values.begin(), values.begin() + 2, values.begin() + 2);
  ASSERT_EQ(values[0], 0);
  ASSERT_EQ(values[1], 0);
  ASSERT_EQ(values[2], 1);
  ASSERT_EQ(values[3], 2);
}

/// @test `swap_range` of a subset of a vector yields the correct values.
TEST(SwapRanges, SwapSubvectors) {
  auto values = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto const reference = std::vector<int>{6, 7, 8, 9, 5, 1, 2, 3, 4};
  arene::base::swap_ranges(values.begin(), values.begin() + 4, values.begin() + 5);
  for (auto itr = reference.cbegin(), itv = values.cbegin(); itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `swap_range` when the destination is the same as the source yields the correct values.
TEST(SwapRanges, SwapEmptyRanges) {
  auto values = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto const reference = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9};
  arene::base::swap_ranges(values.begin(), values.begin(), values.begin() + 5);
  for (auto itr = reference.cbegin(), itv = values.cbegin(); itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `swap_range` of a vector of move-only types.
TEST(SwapRanges, SwapMoveOnlyType) {
  auto const origin = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto values = std::vector<std::unique_ptr<int>>{};
  for (auto const& value : origin) {
    values.emplace_back(std::make_unique<int>(value));
  }

  arene::base::swap_ranges(values.begin(), values.begin() + 4, values.begin() + 5);

  auto const reference = std::vector<int>{6, 7, 8, 9, 5, 1, 2, 3, 4};
  for (auto idx = 0U; idx < origin.size(); ++idx) {
    ASSERT_EQ(*values[idx], reference[idx]);
  }
}

using array8 = arene::base::array<std::uint32_t, 8>;

constexpr auto do_swap_ranges(array8 init) -> array8 {
  auto values = array8{init};
  arene::base::swap_ranges(values.begin(), values.begin() + 3, values.begin() + 4);
  return values;
}

/// @test `swap_range` in a constexpr context.
TEST(SwapRanges, SwapRangesInConstexprContext) {
  static constexpr auto values = arene::base::array<std::uint32_t, 8>{1, 2, 3, 4, 5, 6, 7, 8};

  constexpr auto dst = do_swap_ranges(values);

  static constexpr auto reference = arene::base::array<std::uint32_t, 8>{5, 6, 7, 4, 1, 2, 3, 8};
  static_assert(dst[0] == reference[0], "Can't swap ranges");
  static_assert(dst[1] == reference[1], "Can't swap ranges");
  static_assert(dst[2] == reference[2], "Can't swap ranges");
  static_assert(dst[3] == reference[3], "Can't swap ranges");
  static_assert(dst[4] == reference[4], "Can't swap ranges");
  static_assert(dst[5] == reference[5], "Can't swap ranges");
  static_assert(dst[6] == reference[6], "Can't swap ranges");
  static_assert(dst[7] == reference[7], "Can't swap ranges");
}

/*
 * Tests for noexcept conditions
 */
/// @test `swap_ranges` is noexcept if all operators of the iterator arguments are noexcept.
TEST(SwapRanges, IsNoexceptIfAllNoexcept) {
  static_assert(
      noexcept(arene::base::swap_ranges(noexcept_iterator(), noexcept_iterator(), noexcept_iterator())),
      "Must be noexcept"
  );
}

/// @test `swap_ranges` is not noexcept if the increment operator of the iterator argument throws.
TEST(SwapRanges, IsNotNoexceptIfIncrementThrows) {
  static_assert(
      !noexcept(arene::base::swap_ranges(
          throwing_increment_iterator(),
          throwing_increment_iterator(),
          throwing_increment_iterator()
      )),
      "Must not be noexcept"
  );
}

/// @test `swap_ranges` is not noexcept if the decrement operator of the iterator argument throws.
TEST(SwapRanges, IsNoexceptIfDecrementThrows) {
  static_assert(
      noexcept(arene::base::swap_ranges(
          throwing_decrement_iterator(),
          throwing_decrement_iterator(),
          throwing_decrement_iterator()
      )),
      "Must be noexcept"
  );
}

/// @test `swap_ranges` is not noexcept if the comparison operator of the iterator argument throws.
TEST(SwapRanges, IsNotNoexceptIfComparisonThrows) {
  static_assert(
      !noexcept(arene::base::swap_ranges(
          throwing_comparison_iterator(),
          throwing_comparison_iterator(),
          throwing_comparison_iterator()
      )),
      "Must not be noexcept"
  );
}

/// @test `swap_ranges` is not noexcept if the reference operator of the iterator argument throws.
TEST(SwapRanges, IsNotNoexceptIfReferenceThrows) {
  static_assert(
      !noexcept(arene::base::swap_ranges(
          throwing_reference_iterator(),
          throwing_reference_iterator(),
          throwing_reference_iterator()
      )),
      "Must not be noexcept"
  );
}

/// @test `swap_ranges` is not noexcept if the assignment operator of the iterator argument throws.
TEST(SwapRanges, IsNotNoexceptIfAssignmentThrows) {
  static_assert(
      !noexcept(arene::base::swap_ranges(
          throwing_assignment_iterator(),
          throwing_assignment_iterator(),
          throwing_assignment_iterator()
      )),
      "Must not be noexcept"
  );
}

/// @test `swap_ranges` is not noexcept if all of the operators of the iterator argument throw.
TEST(SwapRanges, IsNotNoexceptIfAllThrows) {
  static_assert(
      !noexcept(arene::base::swap_ranges(throwing_iterator(), throwing_iterator(), throwing_iterator())),
      "Must not be noexcept"
  );
}
}  // namespace
