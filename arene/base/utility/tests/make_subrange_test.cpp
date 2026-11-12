// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/utility/make_subrange.hpp"

#include <gmock/gmock.h>  // IWYU pragma: keep
#include <gtest/gtest.h>  // IWYU pragma: keep

#include "arene/base/array/array.hpp"
#include "arene/base/inline_container/vector.hpp"
#include "arene/base/ranges/enable_borrowed_range.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
constexpr std::size_t capacity = 10;
constexpr std::size_t offset_start = 3;
constexpr std::size_t offset_end = 8;

/// @test Given two pointers, @c arene::base::make_subrange(ptr1,ptr2) returns an iterable view on the sequence
/// @c [ptr1,ptr2) .
TEST(ToRange, CanCreateRangeFromPointers) {
  arene::base::array<int, capacity> arr{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  auto const range = arene::base::make_subrange(&arr[offset_start], &arr[offset_end]);
  ::testing::StaticAssertTypeEq<decltype(*range.begin()), decltype(arr)::reference>();

  arene::base::inline_vector<int, capacity> const expected{arr.begin() + offset_start, arr.begin() + offset_end};
  EXPECT_THAT(range, ::testing::ElementsAreArray(expected));
}
/// @test Given two iterators of category at least forward_iterator, @c arene::base::make_subrange(itr1,itr2) returns an
/// iterable view on the sequence @c [itr1,itr2) .
TEST(ToRange, CanCreateRangeFromIterators) {
  arene::base::inline_vector<int, capacity> vec{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  auto const range = arene::base::make_subrange(vec.begin() + offset_start, vec.begin() + offset_end);
  ::testing::StaticAssertTypeEq<decltype(*range.begin()), decltype(vec)::reference>();

  arene::base::inline_vector<int, capacity> const expected{vec.begin() + offset_start, vec.begin() + offset_end};
  EXPECT_THAT(range, ::testing::ElementsAreArray(expected));
}
/// @test Given two const-iterators of category at least forward_iterator, @c arene::base::make_subrange(itr1,itr2)
/// returns an iterable view on the sequence @c [itr1,itr2) .
TEST(ToRange, CanCreateRangeFromConstIterators) {
  arene::base::inline_vector<int, capacity> const vec{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  auto const range = arene::base::make_subrange(vec.cbegin() + offset_start, vec.cbegin() + offset_end);
  ::testing::StaticAssertTypeEq<decltype(*range.begin()), decltype(vec)::const_reference>();

  arene::base::inline_vector<int, capacity> const expected{vec.begin() + offset_start, vec.begin() + offset_end};

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
  constexpr int arr[capacity] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  STATIC_ASSERT_EQ(sum_range(arene::base::make_subrange(&arr[offset_start], &arr[offset_end])), 25);
}

template <typename T>
class MakeSubrangeIsBorrowedRangeTest : public ::testing::Test {};

using iterator_types =
    ::testing::Types<arene::base::array<int, capacity>::iterator, arene::base::inline_vector<int, capacity>::iterator>;
TYPED_TEST_SUITE(MakeSubrangeIsBorrowedRangeTest, iterator_types, );

/// @test Given an @c iterator_range created by @c arene::base::make_subrange, when @c enable_borrowed_range is
/// queried, then its value is @c true .
TYPED_TEST(MakeSubrangeIsBorrowedRangeTest, IsBorrowedRange) {
  using Iterator = TypeParam;
  using Sentinel = Iterator;
  using Range = arene::base::make_subrange_detail::iterator_range<Iterator, Sentinel>;
  STATIC_ASSERT_TRUE(arene::base::ranges::enable_borrowed_range_v<Range>);
}

}  // namespace
