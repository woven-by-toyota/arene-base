// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/array.hpp"
#include "arene/base/math.hpp"
#include "arene/base/optional.hpp"
#include "arene/base/utility.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/cstdint"
#include "stdlib/include/iterator"
#include "testlibs/minitest/matcher.hpp"
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

class counted_compare {
  std::uint32_t value_;

 public:
  counted_compare() noexcept
      : value_{} {}
  counted_compare(counted_compare const&) = default;
  counted_compare(counted_compare&&) = default;
  auto operator=(counted_compare const&) -> counted_compare& = default;
  auto operator=(counted_compare&&) -> counted_compare& = default;
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  counted_compare(std::uint32_t value) noexcept
      : value_(value) {}

  ~counted_compare() = default;

  auto get_value() const noexcept -> std::uint32_t { return value_; }

  friend auto operator<(counted_compare const& lhs, counted_compare const& rhs) noexcept -> bool {
    ++compare_count;
    return lhs.value_ < rhs.value_;
  }

  friend auto operator==(counted_compare const& lhs, counted_compare const& rhs) noexcept -> bool {
    return lhs.value_ == rhs.value_;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
  static std::uint64_t compare_count;
};

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::uint64_t counted_compare::compare_count{0};

class simple_rng {
  std::uint64_t state_;

 public:
  explicit simple_rng(std::uint64_t state) noexcept
      : state_(state) {}

  auto operator()() noexcept -> std::uint32_t {
    constexpr std::uint64_t multiplier{6364136223846793005};
    constexpr std::uint64_t increment{1442695040888963407};

    state_ = multiplier * state_ + increment;
    return static_cast<std::uint32_t>(state_ & 0XFFFFFFFFU);
  }
};

template <std::size_t SourceSize, std::size_t TargetSize>
struct complexity_sizes {
  static constexpr std::size_t source_size{SourceSize};
  static constexpr std::size_t target_size{TargetSize};
};

using complexity_counts = ::testing::Types<
    complexity_sizes<1024, 128>,
    complexity_sizes<1024, 10>,
    complexity_sizes<1024, 3>,
    complexity_sizes<1024, 900>,
    complexity_sizes<1024, 2000>,
    complexity_sizes<12345, 128>,
    complexity_sizes<12345, 10>,
    complexity_sizes<12345, 9999>,
    complexity_sizes<12345, 12345>,
    complexity_sizes<12345, 20000> >;

template <typename Type>
class PartialSortCopyComplexity : public ::testing::Test {};

TYPED_TEST_SUITE(PartialSortCopyComplexity, complexity_counts, );

/// @test The number of comparisons should be approximately <c>(source range size) * log(min((source range
/// size),(target range size)))</c>. This test checks it is within the range <c>(source range size)</c> and <c>(source
/// range size) * (log2(min((source range size),(target range size))) + 1)</c>
TYPED_TEST(PartialSortCopyComplexity, NLogKComplexity) {
  simple_rng rng{123456789};
  for (std::uint32_t i = 0; i < 10; ++i) {
    arene::base::array<counted_compare, TypeParam::source_size> source{};
    arene::base::array<counted_compare, TypeParam::target_size> target{};

    for (auto& elem : source) {
      elem = rng();
    }

    ASSERT_FALSE(std::is_sorted(source.begin(), source.end()));

    counted_compare::compare_count = 0;

    auto result = std::partial_sort_copy(source.begin(), source.end(), target.begin(), target.end());

    auto const sorted_size{std::min(source.size(), target.size())};

    ASSERT_EQ(result, target.begin() + sorted_size);
    ASSERT_LE(counted_compare::compare_count, source.size() * (arene::base::log2(sorted_size) + 1));
    ASSERT_GE(counted_compare::compare_count, source.size());

    ASSERT_TRUE(std::is_sorted(target.begin(), target.begin() + sorted_size));

    std::sort(source.begin(), source.end());

    if (target.size() <= source.size()) {
      ASSERT_THAT(target, testing::ElementsAreArray(source.begin(), source.begin() + sorted_size));
    } else {
      ASSERT_THAT(source, testing::ElementsAreArray(target.begin(), target.begin() + sorted_size));
      for (auto& elem : arene::base::make_subrange(target.begin() + sorted_size, target.end())) {
        ASSERT_EQ(elem.get_value(), 0);
      }
    }
  }
}

/// @test The number of comparisons should be approximately <c>(source range size) * log(min((source range
/// size),(target range size)))</c> even if the source is already sorted
TYPED_TEST(PartialSortCopyComplexity, NLogKComplexityAlreadySorted) {
  arene::base::array<counted_compare, TypeParam::source_size> source{};
  arene::base::array<counted_compare, TypeParam::target_size> target{};

  std::uint32_t val{0};
  for (auto& elem : source) {
    elem = counted_compare{val};
    ++val;
  }

  ASSERT_TRUE(std::is_sorted(source.begin(), source.end()));

  counted_compare::compare_count = 0;

  auto result = std::partial_sort_copy(source.begin(), source.end(), target.begin(), target.end());

  auto const sorted_size{std::min(source.size(), target.size())};

  ASSERT_EQ(result, target.begin() + sorted_size);
  ASSERT_LE(counted_compare::compare_count, source.size() * (arene::base::log2(sorted_size) + 1));
  ASSERT_GE(counted_compare::compare_count, source.size());

  if (target.size() <= source.size()) {
    ASSERT_THAT(target, testing::ElementsAreArray(source.begin(), source.begin() + sorted_size));
  } else {
    ASSERT_THAT(source, testing::ElementsAreArray(target.begin(), target.begin() + sorted_size));
    for (auto& elem : arene::base::make_subrange(target.begin() + sorted_size, target.end())) {
      ASSERT_EQ(elem.get_value(), 0);
    }
  }
}

/// @test The number of comparisons should be approximately <c>(source range size) * log(min((source range
/// size),(target range size)))</c> even if the source is in reverse order
TYPED_TEST(PartialSortCopyComplexity, NLogKComplexityReverseOrder) {
  arene::base::array<counted_compare, TypeParam::source_size> source{};
  arene::base::array<counted_compare, TypeParam::target_size> target{};

  std::uint32_t val{TypeParam::source_size};
  for (auto& elem : source) {
    elem = counted_compare{val};
    --val;
  }

  ASSERT_TRUE(std::is_sorted(source.rbegin(), source.rend()));

  counted_compare::compare_count = 0;

  auto result = std::partial_sort_copy(source.begin(), source.end(), target.begin(), target.end());

  auto const sorted_size{std::min(source.size(), target.size())};

  ASSERT_EQ(result, target.begin() + sorted_size);

  ASSERT_LE(counted_compare::compare_count, source.size() * (arene::base::log2(sorted_size) + 1));
  ASSERT_GE(counted_compare::compare_count, source.size());

  std::sort(source.begin(), source.end());

  if (target.size() <= source.size()) {
    ASSERT_THAT(target, testing::ElementsAreArray(source.begin(), source.begin() + sorted_size));
  } else {
    ASSERT_THAT(source, testing::ElementsAreArray(target.begin(), target.begin() + sorted_size));
    for (auto& elem : arene::base::make_subrange(target.begin() + sorted_size, target.end())) {
      ASSERT_EQ(elem.get_value(), 0);
    }
  }
}

/// @test The number of comparisons should be approximately <c>(source range size) * log(min((source range
/// size),(target range size)))</c> when using a custom comparator
TYPED_TEST(PartialSortCopyComplexity, NLogKComplexityCustomComparator) {
  simple_rng rng{123456789};

  struct CountingComparator {
    std::uint32_t& count;

    auto operator()(counted_compare const& lhs, counted_compare const& rhs) noexcept -> bool {
      ++count;
      return rhs.get_value() < lhs.get_value();
    }
  };

  for (std::uint32_t i = 0; i < 10; ++i) {
    arene::base::array<counted_compare, TypeParam::source_size> source{};
    arene::base::array<counted_compare, TypeParam::target_size> target{};

    for (auto& elem : source) {
      elem = rng();
    }

    std::uint32_t dummy_count{0};
    ASSERT_FALSE(std::is_sorted(source.begin(), source.end(), CountingComparator{dummy_count}));

    counted_compare::compare_count = 0;
    std::uint32_t count{0};

    auto result =
        std::partial_sort_copy(source.begin(), source.end(), target.begin(), target.end(), CountingComparator{count});

    auto const sorted_size{std::min(source.size(), target.size())};

    ASSERT_EQ(counted_compare::compare_count, 0);
    ASSERT_EQ(result, target.begin() + sorted_size);
    ASSERT_LE(count, source.size() * (arene::base::log2(sorted_size) + 1));
    ASSERT_GE(count, source.size());

    ASSERT_TRUE(std::is_sorted(target.begin(), target.begin() + sorted_size, CountingComparator{dummy_count}));

    std::sort(source.begin(), source.end(), CountingComparator{dummy_count});

    if (target.size() <= source.size()) {
      ASSERT_THAT(target, testing::ElementsAreArray(source.begin(), source.begin() + sorted_size));
    } else {
      ASSERT_THAT(source, testing::ElementsAreArray(target.begin(), target.begin() + sorted_size));
      for (auto& elem : arene::base::make_subrange(target.begin() + sorted_size, target.end())) {
        ASSERT_EQ(elem.get_value(), 0);
      }
    }
  }
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
