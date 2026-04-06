// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/utility/make_subrange.hpp"

#include <array>
#include <cstddef>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {
constexpr std::size_t offset_start = 3;
constexpr std::size_t offset_end = 8;

/// @test Given two pointers, @c arene::base::make_subrange(ptr1,ptr2) returns an iterable view on the sequence
/// @c [ptr1,ptr2) .
TEST(ToRange, CanCreateRangeFromPointers) {
  std::array<int, 10> arr{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  auto const range = arene::base::make_subrange(&arr.at(offset_start), &arr.at(offset_end));
  ::testing::StaticAssertTypeEq<decltype(*range.begin()), decltype(arr)::reference>();

  std::vector<int> const expected{arr.begin() + offset_start, arr.begin() + offset_end};
  EXPECT_THAT(range, ::testing::ElementsAreArray(expected));
}
/// @test Given two iterators of category at least forward_iterator, @c arene::base::make_subrange(itr1,itr2) returns an
/// iterable view on the sequence @c [itr1,itr2) .
TEST(ToRange, CanCreateRangeFromIterators) {
  std::vector<int> vec{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  auto const range = arene::base::make_subrange(vec.begin() + offset_start, vec.begin() + offset_end);
  ::testing::StaticAssertTypeEq<decltype(*range.begin()), decltype(vec)::reference>();

  std::vector<int> const expected{vec.begin() + offset_start, vec.begin() + offset_end};
  EXPECT_THAT(range, ::testing::ElementsAreArray(expected));
}
/// @test Given two const-iterators of category at least forward_iterator, @c arene::base::make_subrange(itr1,itr2)
/// returns an iterable view on the sequence @c [itr1,itr2) .
TEST(ToRange, CanCreateRangeFromConstIterators) {
  std::vector<int> const vec{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  auto const range = arene::base::make_subrange(vec.cbegin() + offset_start, vec.cbegin() + offset_end);
  ::testing::StaticAssertTypeEq<decltype(*range.begin()), decltype(vec)::const_reference>();

  std::vector<int> const expected{vec.begin() + offset_start, vec.begin() + offset_end};

  EXPECT_THAT(range, ::testing::ElementsAreArray(expected));
}

namespace {
template <typename Range>
constexpr auto sum_range(Range&& range) -> int {
  int sum = 0;
  for (auto& elem : range) {
    sum += elem;
  }
  return sum;
}
}  // namespace
/// @test Validates @c arene::base::make_subrange works in constexpr.
TEST(ToRange, CanCreateConstexprIterableRange) {
  // NOLINTNEXTLINE(hicpp-avoid-c-arrays)
  constexpr int arr[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  STATIC_ASSERT_EQ(sum_range(arene::base::make_subrange(&arr[offset_start], &arr[offset_end])), 25);
}
}  // namespace
