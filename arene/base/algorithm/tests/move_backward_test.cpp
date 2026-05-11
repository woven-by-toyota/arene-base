// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/move_backward.hpp"

#include <cstdint>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "arene/base/algorithm/tests/test_iterators.hpp"
#include "arene/base/array.hpp"
#include "arene/base/compare.hpp"
#include "arene/base/detail/wrapped_iterator.hpp"

namespace {

/// @test `move_backward` an empty vector results in an empty destination vector.
TEST(MoveBackward, CanMoveEmptyVector) {
  std::vector<int> values;
  std::vector<int> dst;
  arene::base::move_backward(values.begin(), values.end(), dst.begin());
}

/// @test `move_backward` an empty vector to another vector won't change the destination vector.
TEST(MoveBackward, MoveEmptyToAnotherVector) {
  auto const values = std::vector<int>{};
  auto dst = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::move_backward(values.begin(), values.end(), dst.begin());
  ASSERT_EQ(dst.front(), 1);
}

/// @test `move_backward` an empty vector to itself results in an empty vector.
TEST(MoveBackward, MoveEmptyToSelf) {
  auto values = std::vector<int>{};
  arene::base::move_backward(values.begin(), values.end(), values.begin());
  ASSERT_TRUE(values.empty());
}

/// @test `move_backward` a single element vector to another vector results in the destination vector containing
/// the value.
TEST(MoveBackward, CanMoveOneElementVector) {
  constexpr auto value = 42;
  auto const values = std::vector<int>{value};
  auto dst = std::vector<int>{0};
  arene::base::move_backward(values.begin(), values.end(), dst.end());
  ASSERT_EQ(dst.front(), value);
}

/// @test `move_backward` a subset of a vector to the end of itself correctly relocates values.
TEST(MoveBackward, MoveToSameVector) {
  auto values = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  auto const reference = std::vector<int>{-1, -1, -1, 4, 5, 1, 2, 3};
  constexpr auto shift = 3U;
  arene::base::move_backward(values.begin(), values.begin() + shift, values.end());
  for (auto itr = reference.cbegin() + shift, itv = values.cbegin() + shift; itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `move_backward` a subset of a vector to itself at different but overlapping offsets,
/// resulting in the correct values in the vector.
TEST(MoveBackward, MoveOverlappingRanges) {
  auto values = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  auto const reference = std::vector<int>{-1, -1, -1, 1, 2, 3, 4, 5};
  arene::base::move_backward(values.begin(), values.begin() + 5, values.end());
  for (auto itr = reference.cbegin() + 3, itv = values.cbegin() + 3; itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `move_backward` a vector to another vector results in the destination vector containing the values.
TEST(MoveBackward, MoveToAnotherVector) {
  auto const values = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  auto dst = std::vector<int>{0, 0, 0, 0, 0, 0, 0, 0};
  auto const reference = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::move_backward(values.begin(), values.end(), dst.end());
  for (auto itr = reference.cbegin(), itv = dst.cbegin(); itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `move_backward` vector of move-only type.
TEST(MoveBackward, MoveMoveOnlyType) {
  auto const origin = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  auto values = std::vector<std::unique_ptr<int>>{};
  auto dst = std::vector<std::unique_ptr<int>>{};
  for (auto const& value : origin) {
    values.emplace_back(std::make_unique<int>(value));
    dst.emplace_back(std::make_unique<int>(0));
  }

  arene::base::move_backward(values.begin(), values.end(), dst.end());

  for (auto idx = 0U; idx < origin.size(); ++idx) {
    ASSERT_EQ(*dst[idx], origin[idx]);
  }
}

namespace {

using array8 = arene::base::array<std::uint32_t, 8>;

constexpr auto do_move_backward(array8 values) -> array8 {
  auto dst = array8{};
  arene::base::move_backward(values.begin(), values.end(), dst.end());
  return dst;
}

}  // namespace

/// @test `move_backward` in a constexpr context.
TEST(MoveBackward, MoveInConstexprContext) {
  static constexpr arene::base::array<std::uint32_t, 8> values{{1, 2, 3, 4, 5, 6, 7, 8}};

  constexpr auto dst = do_move_backward(values);

  for (auto itr = dst.cbegin(), itv = dst.cbegin(); itr < dst.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/*
 * Tests for noexcept conditions
 */
/// @test Verify `move_backward` is noexcept if all operators of iterator arguments are noexcept.
TEST(MoveBackward, IsNoexceptIfAllNoexcept) {
  static_assert(
      noexcept(arene::base::move_backward(noexcept_iterator(), noexcept_iterator(), noexcept_iterator())),
      "Must be noexcept"
  );
}

/// @test Verify `move_backward` is not noexcept if increment operator of iterator arguments is not noexcept.
TEST(MoveBackward, IsNoexceptIfIncrementThrows) {
  static_assert(
      noexcept(arene::base::move_backward(
          throwing_increment_iterator(),
          throwing_increment_iterator(),
          throwing_increment_iterator()
      )),
      "Must be noexcept"
  );
}

/// @test Verify `move_backward` is not noexcept if decrement operator of iterator arguments is not noexcept.
TEST(MoveBackward, IsNotNoexceptIfDecrementThrows) {
  static_assert(
      !noexcept(arene::base::move_backward(
          throwing_decrement_iterator(),
          throwing_decrement_iterator(),
          throwing_decrement_iterator()
      )),
      "Must not be noexcept"
  );
}

/// @test Verify `move_backward` is not noexcept if comparison operator of iterator arguments is not noexcept.
TEST(MoveBackward, IsNotNoexceptIfComparisonThrows) {
  static_assert(
      !noexcept(arene::base::move_backward(
          throwing_comparison_iterator(),
          throwing_comparison_iterator(),
          throwing_comparison_iterator()
      )),
      "Must not be noexcept"
  );
}

/// @test Verify `move_backward` is not noexcept if reference operator of iterator arguments is not noexcept.
TEST(MoveBackward, IsNotNoexceptIfReferenceThrows) {
  static_assert(
      !noexcept(arene::base::move_backward(
          throwing_reference_iterator(),
          throwing_reference_iterator(),
          throwing_reference_iterator()
      )),
      "Must not be noexcept"
  );
}

/// @test Verify `move_backward` is not noexcept if assignment operator of iterator arguments is not noexcept.
TEST(MoveBackward, IsNotNoexceptIfAssignmentThrows) {
  static_assert(
      !noexcept(arene::base::move_backward(
          throwing_assignment_iterator(),
          throwing_assignment_iterator(),
          throwing_assignment_iterator()
      )),
      "Must not be noexcept"
  );
}

/// @test Verify `move_backward` is not noexcept if all operators of iterator arguments throw.
TEST(MoveBackward, IsNotNoexceptIfAllThrows) {
  static_assert(
      !noexcept(arene::base::move_backward(throwing_iterator(), throwing_iterator(), throwing_iterator())),
      "Must not be noexcept"
  );
}
}  // namespace
