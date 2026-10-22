// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/array.hpp"
#include "arene/base/optional.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/cstdint"
#include "stdlib/include/iterator"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {
/// @test Passing an empty source range to @c partial_sort_copy copies no elements, and returns the start of the output
/// range
TEST(PartialSortCopy, AnEmptySourceRangeDoesNothing) {
  arene::base::array<std::int32_t const, 5> source{1, 2, 3, 4, 5};
  arene::base::array<std::int32_t, 5> target{0, 0, 0, 0, 0};

  auto result = std::partial_sort_copy(source.begin(), source.begin(), target.begin(), target.end());

  ASSERT_EQ(result, target.begin());

  for (auto& elem : target) {
    ASSERT_EQ(elem, 0);
  }
}

/// @test Passing an source range of a single item with a big enough output range to @c partial_sort_copy copies that
/// element, and returns the incremented output iterator
TEST(PartialSortCopy, ASingleElementIsCopied) {
  arene::base::array<std::int32_t const, 5> source{1, 2, 3, 4, 5};
  arene::base::array<std::int32_t, 5> target{0, 0, 0, 0, 0};

  auto result = std::partial_sort_copy(source.begin(), source.begin() + 1, target.begin(), target.end());

  ASSERT_EQ(result, target.begin() + 1);

  ASSERT_EQ(target[0], 1);
  ASSERT_EQ(target[1], 0);
  ASSERT_EQ(target[2], 0);
  ASSERT_EQ(target[3], 0);
  ASSERT_EQ(target[4], 0);
}

/// @test Passing a non-empty source range and an empty output range to @c partial_sort_copy does nothing and
/// returns the start of the output range
TEST(PartialSortCopy, AnEmptyOutputRangeAcceptsNoValues) {
  arene::base::array<std::int32_t const, 5> source{1, 2, 3, 4, 5};
  arene::base::array<std::int32_t, 5> target{0, 0, 0, 0, 0};

  auto result = std::partial_sort_copy(source.begin(), source.end(), target.begin(), target.begin());

  ASSERT_EQ(result, target.begin());

  for (auto& elem : target) {
    ASSERT_EQ(elem, 0);
  }
}

/// @test Passing a non-empty source range and an output range of one value to @c partial_sort_copy copies the smallest
/// element, and returns the incremented output iterator
TEST(PartialSortCopy, SmallestElementForSingleOutputValue) {
  arene::base::array<std::int32_t const, 5> source{4, 5, 2, 1, 3};
  arene::base::array<std::int32_t, 5> target{0, 0, 0, 0, 0};

  auto result = std::partial_sort_copy(source.begin(), source.end(), target.begin(), target.begin() + 1);

  ASSERT_EQ(result, target.begin() + 1);

  ASSERT_EQ(target[0], 1);
  ASSERT_EQ(target[1], 0);
  ASSERT_EQ(target[2], 0);
  ASSERT_EQ(target[3], 0);
  ASSERT_EQ(target[4], 0);
}

/// @test Passing a source of more than elements and an output range of two values to @c partial_sort_copy copies the
/// smallest two elements in order, and returns the twice-incremented output iterator
TEST(PartialSortCopy, SmallestTwoElementsForTwoOutputValues) {
  arene::base::array<std::int32_t const, 5> source{4, 5, 2, 1, 3};
  arene::base::array<std::int32_t, 5> target{0, 0, 0, 0, 0};

  auto result = std::partial_sort_copy(source.begin(), source.end(), target.begin(), target.begin() + 2);

  ASSERT_EQ(result, target.begin() + 2);

  ASSERT_EQ(target[0], 1);
  ASSERT_EQ(target[1], 2);
  ASSERT_EQ(target[2], 0);
  ASSERT_EQ(target[3], 0);
  ASSERT_EQ(target[4], 0);
}

/// @test Passing a source of more than elements with an input iterator and an output range of two values to @c
/// partial_sort_copy copies the smallest two elements in order, and returns the twice-incremented output iterator
TEST(PartialSortCopy, SmallestTwoElementsFromInputIteratorForTwoOutputValues) {
  arene::base::array<std::int32_t const, 5> source{4, 5, 2, 1, 3};
  arene::base::array<std::int32_t, 5> target{0, 0, 0, 0, 0};

  auto result = std::partial_sort_copy(
      testing::make_demoted_iterator<std::input_iterator_tag>(source.begin()),
      testing::make_demoted_iterator<std::input_iterator_tag>(source.end()),
      target.begin(),
      target.begin() + 2
  );

  ASSERT_EQ(result, target.begin() + 2);

  ASSERT_EQ(target[0], 1);
  ASSERT_EQ(target[1], 2);
  ASSERT_EQ(target[2], 0);
  ASSERT_EQ(target[3], 0);
  ASSERT_EQ(target[4], 0);
}

/// @test Passing a source of more than elements with a forward iterator and an output range of two values to @c
/// partial_sort_copy copies the smallest two elements in order, and returns the twice-incremented output iterator
TEST(PartialSortCopy, SmallestTwoElementsFromForwardIteratorForTwoOutputValues) {
  arene::base::array<std::int32_t const, 5> source{4, 5, 2, 1, 3};
  arene::base::array<std::int32_t, 5> target{0, 0, 0, 0, 0};

  auto result = std::partial_sort_copy(
      testing::make_demoted_iterator<std::forward_iterator_tag>(source.begin()),
      testing::make_demoted_iterator<std::forward_iterator_tag>(source.end()),
      target.begin(),
      target.begin() + 2
  );

  ASSERT_EQ(result, target.begin() + 2);

  ASSERT_EQ(target[0], 1);
  ASSERT_EQ(target[1], 2);
  ASSERT_EQ(target[2], 0);
  ASSERT_EQ(target[3], 0);
  ASSERT_EQ(target[4], 0);
}

/// @test Passing a source of more than elements with a forward iterator and an output range of a different type to @c
/// partial_sort_copy copies the smallest two elements in order, and returns the twice-incremented output iterator
TEST(PartialSortCopy, SmallestTwoElementsWithDifferentValueTypes) {
  arene::base::array<std::int32_t const, 5> source{4, 5, 2, 1, 3};
  arene::base::array<std::int64_t, 5> target{0, 0, 0, 0, 0};

  auto result = std::partial_sort_copy(
      testing::make_demoted_iterator<std::forward_iterator_tag>(source.begin()),
      testing::make_demoted_iterator<std::forward_iterator_tag>(source.end()),
      target.begin(),
      target.begin() + 2
  );

  ASSERT_EQ(result, target.begin() + 2);

  ASSERT_EQ(target[0], 1);
  ASSERT_EQ(target[1], 2);
  ASSERT_EQ(target[2], 0);
  ASSERT_EQ(target[3], 0);
  ASSERT_EQ(target[4], 0);
}

/// @test Passing a source of more than elements with a bidirectional iterator and an output range of two values to @c
/// partial_sort_copy copies the smallest two elements in order, and returns the twice-incremented output iterator
TEST(PartialSortCopy, SmallestTwoElementsFromBidirectionalIteratorForTwoOutputValues) {
  arene::base::array<std::int32_t const, 5> source{4, 5, 2, 1, 3};
  arene::base::array<std::int32_t, 5> target{0, 0, 0, 0, 0};

  auto result = std::partial_sort_copy(
      testing::make_demoted_iterator<std::bidirectional_iterator_tag>(source.begin()),
      testing::make_demoted_iterator<std::bidirectional_iterator_tag>(source.end()),
      target.begin(),
      target.begin() + 2
  );

  ASSERT_EQ(result, target.begin() + 2);

  ASSERT_EQ(target[0], 1);
  ASSERT_EQ(target[1], 2);
  ASSERT_EQ(target[2], 0);
  ASSERT_EQ(target[3], 0);
  ASSERT_EQ(target[4], 0);
}

/// @test Passing a source of some elements and an output range with more elements to @c partial_sort_copy copies source
/// elements in sorted order, and returns an iterator referring to the end of the copied elements
TEST(PartialSortCopy, CopiesRangeInSortedOrderIfLargerDestination) {
  arene::base::array<std::int32_t const, 5> source{4, 5, 2, 1, 3};
  arene::base::array<std::int32_t, 10> target{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  auto result = std::partial_sort_copy(source.begin(), source.end(), target.begin(), target.end());

  ASSERT_EQ(result, target.begin() + source.size());

  ASSERT_EQ(target[0], 1);
  ASSERT_EQ(target[1], 2);
  ASSERT_EQ(target[2], 3);
  ASSERT_EQ(target[3], 4);
  ASSERT_EQ(target[4], 5);
  ASSERT_EQ(target[5], 0);
  ASSERT_EQ(target[6], 0);
  ASSERT_EQ(target[7], 0);
  ASSERT_EQ(target[8], 0);
  ASSERT_EQ(target[9], 0);
}

/// @test Passing a source with duplicates and a smaller output to @c partial_sort_copy copies the smallest elements in
/// order, and returns the incremented output iterator
TEST(PartialSortCopy, SmallestElementsFromRangeWithDuplicates) {
  arene::base::array<std::int32_t const, 10> source{4, 5, 2, 1, 3, 1, 1, 3, 5, 6};
  arene::base::array<std::int32_t, 5> target{0, 0, 0, 0, 0};

  auto result = std::partial_sort_copy(
      testing::make_demoted_iterator<std::input_iterator_tag>(source.begin()),
      testing::make_demoted_iterator<std::input_iterator_tag>(source.end()),
      target.begin(),
      target.end()
  );

  ASSERT_EQ(result, target.end());

  ASSERT_EQ(target[0], 1);
  ASSERT_EQ(target[1], 1);
  ASSERT_EQ(target[2], 1);
  ASSERT_EQ(target[3], 2);
  ASSERT_EQ(target[4], 3);
}

class empty_on_move {
  arene::base::optional<std::int32_t> value_;

 public:
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  empty_on_move(std::int32_t value) noexcept
      : value_(value) {}
  empty_on_move(empty_on_move&& other) noexcept
      : value_(std::move(other.value_)) {
    ARENE_PRECONDITION(other.has_value());
    other.value_.reset();
  }
  empty_on_move(empty_on_move const&) = delete;

  ~empty_on_move() = default;

  auto operator=(empty_on_move const&) -> empty_on_move& = delete;
  auto operator=(empty_on_move&& other) noexcept -> empty_on_move& {
    ARENE_PRECONDITION(other.has_value());
    value_ = std::move(other.value_);
    other.value_.reset();
    return *this;
  }

  auto has_value() const noexcept -> bool { return value_.has_value(); }
  auto get_value() const noexcept -> std::int32_t { return *value_; }

  friend auto operator<(empty_on_move const& lhs, empty_on_move const& rhs) noexcept -> bool {
    return lhs.value_ < rhs.value_;
  }
};

/// @test Invoking @c partial_sort_copy with move iterators should move the values
TEST(PartialSortCopy, MoveIterators) {
  arene::base::array<empty_on_move, 10> source{4, 50, 20, 1, 30, 10, 11, 3, 2, 5};
  arene::base::array<empty_on_move, 5> target{0, 0, 0, 0, 0};

  auto result = std::partial_sort_copy(
      std::make_move_iterator(source.begin()),
      std::make_move_iterator(source.end()),
      target.begin(),
      target.end()
  );

  ASSERT_EQ(result, target.end());

  for (auto& elem : source) {
    ASSERT_FALSE(elem.has_value());
  }
  ASSERT_EQ(target[0].get_value(), 1);
  ASSERT_EQ(target[1].get_value(), 2);
  ASSERT_EQ(target[2].get_value(), 3);
  ASSERT_EQ(target[3].get_value(), 4);
  ASSERT_EQ(target[4].get_value(), 5);
}

}  // namespace
