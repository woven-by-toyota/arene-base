// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/move.hpp"

#include <cstdint>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "arene/base/algorithm/tests/test_iterators.hpp"
#include "arene/base/array.hpp"
#include "arene/base/compare.hpp"
#include "arene/base/detail/wrapped_iterator.hpp"

namespace {

/// @test `move` an empty vector to an empty destination vector leaves the destination vector empty.
TEST(Move, CanMoveEmptyVector) {
  std::vector<int> values;
  std::vector<int> dst;
  arene::base::move(values.begin(), values.end(), dst.begin());
}

/// @test `move` an empty vector to another vector won't change the destination vector.
TEST(Move, MoveEmptyToAnotherVector) {
  auto const values = std::vector<int>{};
  auto dst = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  arene::base::move(values.begin(), values.end(), dst.begin());
  ASSERT_EQ(dst.front(), 1);
}

/// @test `move` an empty vector to itself results in an empty vector.
TEST(Move, MoveEmptyToSelf) {
  auto values = std::vector<int>{};
  arene::base::move(values.begin(), values.end(), values.begin());
  ASSERT_TRUE(values.empty());
}

/// @test `move` a single element vector to another vector results in the destination vector containing the value.
TEST(Move, CanMoveOneElementVector) {
  constexpr auto value = 42;
  auto const values = std::vector<int>{value};
  auto dst = std::vector<int>{0};
  arene::base::move(values.begin(), values.end(), dst.begin());
  ASSERT_EQ(dst.front(), value);
}

/// @test `move` a subset of a vector to itself at different offsets,
/// resulting in the correct values in the vector.
TEST(Move, MoveToSameVector) {
  auto values = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  auto const reference = std::vector<int>{-1, -1, -1, 4, 5, 1, 2, 3};
  constexpr auto shift = 3U;
  arene::base::move(values.begin(), values.begin() + shift, values.end() - shift);
  for (auto itr = reference.cbegin() + shift, itv = values.cbegin() + shift; itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `move` a vector to another vector results in the destination vector containing the values.
TEST(Move, MoveToAnotherVector) {
  auto const values = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  auto dst = std::vector<int>{0, 0, 0, 0, 0, 0, 0, 0};

  arene::base::move(values.begin(), values.end(), dst.begin());

  auto const reference = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  for (auto itr = reference.cbegin(), itv = dst.cbegin(); itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `move` a vector to another vector with move-only type.
TEST(Move, MoveMoveOnlyType) {
  auto const origin = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8};
  auto values = std::vector<std::unique_ptr<int>>{};
  auto dst = std::vector<std::unique_ptr<int>>{};
  for (auto const& value : origin) {
    values.emplace_back(std::make_unique<int>(value));
    dst.emplace_back(std::make_unique<int>(0));
  }

  arene::base::move(values.begin(), values.end(), dst.begin());

  for (auto idx = 0U; idx < origin.size(); ++idx) {
    ASSERT_EQ(*dst[idx], origin[idx]);
  }
}

namespace {

using array8 = arene::base::array<std::uint32_t, 8>;

constexpr auto do_move(array8 values) -> array8 {
  auto dst = array8{};
  arene::base::move(values.begin(), values.end(), dst.begin());
  return dst;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
constexpr auto do_move(array8 init, array8 values, unsigned values_offset, unsigned len, unsigned dst_offset)
    -> array8 {
  auto dst = array8(init);
  arene::base::move(values.begin() + values_offset, values.begin() + values_offset + len, dst.begin() + dst_offset);
  return dst;
}

}  // namespace

/// @test `move` relocates an entire collection of numbers in a constexpr context to a destination collection.
TEST(Move, MoveInConstexprContext) {
  static constexpr arene::base::array<std::uint32_t, 8> values{{1, 2, 3, 4, 5, 6, 7, 8}};

  constexpr auto dst = do_move(values);

  for (auto itr = dst.cbegin(), itv = dst.cbegin(); itr < dst.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/// @test `move` a subset of an array of numbers in a constexpr context into existing elements in another array.
TEST(Move, MovePartialInConstexprContext) {
  static constexpr auto values = arene::base::array<std::uint32_t, 8>{{1, 2, 3, 4, 5, 6, 7, 8}};
  static constexpr auto initial = arene::base::array<std::uint32_t, 8>{{101, 102, 103, 104, 105, 106, 107, 108}};

  constexpr auto dst = do_move(initial, values, 2, 4, 3);

  static auto const reference = arene::base::array<std::uint32_t, 8>{101, 102, 103, 3, 4, 5, 6, 108};
  for (auto itr = reference.cbegin(), itv = dst.cbegin(); itr < reference.end(); ++itr, ++itv) {
    ASSERT_EQ(*itr, *itv);
  }
}

/*
 * Tests for noexcept conditions
 */
/// @test Verify `move` is noexcept if all of the operators of the iterator arguments are noexcept.
TEST(Move, IsNoexceptIfAllNoexcept) {
  static_assert(
      noexcept(arene::base::move(noexcept_iterator(), noexcept_iterator(), noexcept_iterator())),
      "Must be noexcept"
  );
}

/// @test Verify `move` is not noexcept if increment operator of the iterator arguments is not noexcept.
TEST(Move, IsNotNoexceptIfIncrementThrows) {
  static_assert(
      !noexcept(
          arene::base::move(throwing_increment_iterator(), throwing_increment_iterator(), throwing_increment_iterator())
      ),
      "Must not be noexcept"
  );
}

/// @test Verify `move` is not noexcept if decrement operator of the iterator arguments is not noexcept.
TEST(Move, IsNoexceptIfDecrementThrows) {
  static_assert(
      noexcept(
          arene::base::move(throwing_decrement_iterator(), throwing_decrement_iterator(), throwing_decrement_iterator())
      ),
      "Must be noexcept"
  );
}

/// @test Verify `move` is not noexcept if comparison operator of the iterator arguments is not noexcept.
TEST(Move, IsNotNoexceptIfComparisonThrows) {
  static_assert(
      !noexcept(arene::base::move(
          throwing_comparison_iterator(),
          throwing_comparison_iterator(),
          throwing_comparison_iterator()
      )),
      "Must not be noexcept"
  );
}

/// @test Verify `move` is not noexcept if reference operator of the iterator arguments is not noexcept.
TEST(Move, IsNotNoexceptIfReferenceThrows) {
  static_assert(
      !noexcept(
          arene::base::move(throwing_reference_iterator(), throwing_reference_iterator(), throwing_reference_iterator())
      ),
      "Must not be noexcept"
  );
}

/// @test Verify `move` is not noexcept if assignment operator of the iterator arguments is not noexcept.
TEST(Move, IsNotNoexceptIfAssignmentThrows) {
  static_assert(
      !noexcept(arene::base::move(
          throwing_assignment_iterator(),
          throwing_assignment_iterator(),
          throwing_assignment_iterator()
      )),
      "Must not be noexcept"
  );
}

/// @test Verify `move` is not noexcept if all of the operators of the iterator arguments throw.
TEST(Move, IsNotNoexceptIfAllThrows) {
  static_assert(
      !noexcept(arene::base::move(throwing_iterator(), throwing_iterator(), throwing_iterator())),
      "Must not be noexcept"
  );
}
}  // namespace
