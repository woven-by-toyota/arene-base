// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iterator_types.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/cstdint"
#include "stdlib/include/initializer_list"
#include "stdlib/include/iterator"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

class local_with_reverse_ordering {
  std::uint32_t value_;

 public:
  explicit constexpr local_with_reverse_ordering(std::uint32_t val) noexcept
      : value_(val) {}

  friend constexpr auto
  operator<(local_with_reverse_ordering const& lhs, local_with_reverse_ordering const& rhs) noexcept -> bool {
    return rhs.value_ < lhs.value_;
  }
};

class type_with_throwing_comparison {
 public:
  friend constexpr auto operator<(type_with_throwing_comparison const&, type_with_throwing_comparison const&) noexcept(
      false
  ) -> bool {
    return false;
  }
};

/// @test is_sorted returns a boolean
TEST(IsSorted, IsSortedTakesAnIteratorPairAndReturnsABoolean) {
  testing::StaticAssertTypeEq<
      bool,
      decltype(std::is_sorted(std::declval<std::uint32_t*>(), std::declval<std::uint32_t*>()))>();
  testing::StaticAssertTypeEq<
      bool,
      decltype(std::is_sorted(
          std::declval<local_with_reverse_ordering*>(),
          std::declval<local_with_reverse_ordering*>()
      ))>();
}

/// @test If elements are out of order, the range is not sorted
TEST(IsSorted, AnOutOfOrderArrayIsNotSorted) {
  std::initializer_list<std::uint32_t> const data{1, 4, 3, 2};

  ASSERT_FALSE(std::is_sorted(std::begin(data), std::end(data)));
}

/// @test A range of elements in increasing order is sorted
TEST(IsSorted, AnInOrderArrayIsSorted) {
  std::initializer_list<std::uint32_t> const data{10, 20, 35, 99};

  ASSERT_TRUE(std::is_sorted(std::begin(data), std::end(data)));
}

/// @test An empty range is sorted
TEST(IsSorted, AnEmptyRangeIsSorted) {
  std::initializer_list<std::uint32_t> const data{};

  ASSERT_TRUE(std::is_sorted(std::begin(data), std::begin(data)));
}

/// @test An empty range of UDTs is sorted
TEST(IsSorted, AnEmptyRangeOfUDTsIsSorted) {
  type_with_throwing_comparison const data{};

  ASSERT_TRUE(std::is_sorted(&data, &data));
}

/// @test A single-element range is sorted
TEST(IsSorted, ASingleElementRangeIsSorted) {
  std::uint32_t const data = 42;

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_TRUE(std::is_sorted(&data, &data + 1));
}

/// @test A range with adjacent equal elements is still sorted
TEST(IsSorted, EqualElementsAreAllowedInASortedSequence) {
  std::initializer_list<std::uint32_t> const data{10, 20, 20, 99};

  ASSERT_TRUE(std::is_sorted(std::begin(data), std::end(data)));
}

/// @test If elements are out of order, the range is not sorted
TEST(IsSorted, AnOutOfOrderUDTArrayIsNotSorted) {
  std::initializer_list<local_with_reverse_ordering> const data{
      local_with_reverse_ordering{1},
      local_with_reverse_ordering{4},
      local_with_reverse_ordering{3},
      local_with_reverse_ordering{2}
  };

  ASSERT_FALSE(std::is_sorted(std::begin(data), std::end(data)));
}

/// @test A range of elements in increasing order is sorted
TEST(IsSorted, AnInOrderUDTArrayIsSorted) {
  std::initializer_list<local_with_reverse_ordering> const data{
      local_with_reverse_ordering{99},
      local_with_reverse_ordering{65},
      local_with_reverse_ordering{35},
      local_with_reverse_ordering{10}
  };

  ASSERT_TRUE(std::is_sorted(std::begin(data), std::end(data)));
}

/// @test An empty range is sorted
TEST(IsSorted, AnEmptyUDTRangeIsSorted) {
  std::initializer_list<local_with_reverse_ordering> const data{};

  ASSERT_TRUE(std::is_sorted(std::begin(data), std::begin(data)));
}

/// @test A single-element range is sorted
TEST(IsSorted, ASingleElementUDTRangeIsSorted) {
  local_with_reverse_ordering const data{42};

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ASSERT_TRUE(std::is_sorted(&data, &data + 1));
}

/// @test A range with adjacent equal elements is still sorted
TEST(IsSorted, EqualElementsAreAllowedInASortedSequenceOfUDTs) {
  std::initializer_list<local_with_reverse_ordering> const data{
      local_with_reverse_ordering{99},
      local_with_reverse_ordering{20},
      local_with_reverse_ordering{20},
      local_with_reverse_ordering{10}
  };

  ASSERT_TRUE(std::is_sorted(std::begin(data), std::end(data)));
}

class opposite_sorting {
 public:
  template <typename Arg>
  constexpr auto operator()(Arg const& lhs, Arg const& rhs) -> bool {
    return rhs < lhs;
  }
};

/// @test A range sorted by default comparator might not be sorted by custom comparator
TEST(IsSorted, RangeSortedByLessNotSortedByCustomCompare) {
  std::initializer_list<local_with_reverse_ordering> const data{
      local_with_reverse_ordering{99},
      local_with_reverse_ordering{20},
      local_with_reverse_ordering{20},
      local_with_reverse_ordering{10}
  };

  // Note: default ordering of @c local_with_reverse_ordering requires the values in decreasing order, so @c
  // opposite_ordering would require the values to be in ascending order
  ASSERT_TRUE(std::is_sorted(std::begin(data), std::end(data)));
  ASSERT_FALSE(std::is_sorted(std::begin(data), std::end(data), opposite_sorting{}));
}

/// @test A range sorted by custom comparator is sorted
TEST(IsSorted, RangeSortedByCustomCompare) {
  std::initializer_list<local_with_reverse_ordering> const data{
      local_with_reverse_ordering{10},
      local_with_reverse_ordering{20},
      local_with_reverse_ordering{20},
      local_with_reverse_ordering{99}
  };

  ASSERT_FALSE(std::is_sorted(std::begin(data), std::end(data)));
  ASSERT_TRUE(std::is_sorted(std::begin(data), std::end(data), opposite_sorting{}));
}

struct non_throwing_comparison {
  template <typename T>
  auto operator()(T const&, T const&) noexcept -> bool;
};

struct throwing_comparison {
  template <typename T>
  auto operator()(T const&, T const&) noexcept(false) -> bool;
};

/// @test @c is_sorted is @c noexcept if all operations are @c noexcept
TEST(IsSorted, IsNoexceptIfAllOperationsAreNoexcept) {
  static_assert(
      noexcept(
          std::is_sorted(std::declval<local_with_reverse_ordering*>(), std::declval<local_with_reverse_ordering*>())
      ),
      "Must be noexcept"
  );
  static_assert(
      noexcept(std::is_sorted(
          std::declval<testing::non_throwing_forward_iterator<local_with_reverse_ordering>>(),
          std::declval<testing::non_throwing_forward_iterator<local_with_reverse_ordering>>()
      )),
      "Must be noexcept"
  );
  static_assert(
      noexcept(std::is_sorted(std::declval<std::int32_t*>(), std::declval<std::int32_t*>())),
      "Must be noexcept"
  );
  static_assert(
      noexcept(std::is_sorted(
          std::declval<testing::non_throwing_forward_iterator<std::uint32_t>>(),
          std::declval<testing::non_throwing_forward_iterator<std::uint32_t>>()
      )),
      "Must be noexcept"
  );

  static_assert(
      noexcept(std::is_sorted(
          std::declval<local_with_reverse_ordering*>(),
          std::declval<local_with_reverse_ordering*>(),
          std::declval<non_throwing_comparison>()
      )),
      "Must be noexcept"
  );
  static_assert(
      noexcept(std::is_sorted(
          std::declval<testing::non_throwing_forward_iterator<local_with_reverse_ordering>>(),
          std::declval<testing::non_throwing_forward_iterator<local_with_reverse_ordering>>(),
          std::declval<non_throwing_comparison>()
      )),
      "Must be noexcept"
  );
  static_assert(
      noexcept(std::is_sorted(
          std::declval<std::int32_t*>(),
          std::declval<std::int32_t*>(),
          std::declval<non_throwing_comparison>()
      )),
      "Must be noexcept"
  );
  static_assert(
      noexcept(std::is_sorted(
          std::declval<testing::non_throwing_forward_iterator<std::uint32_t>>(),
          std::declval<testing::non_throwing_forward_iterator<std::uint32_t>>(),
          std::declval<non_throwing_comparison>()
      )),
      "Must be noexcept"
  );
}

/// @test @c is_sorted is not @c noexcept if iterator operations are not @c noexcept
TEST(IsSorted, IsNotNoexceptIfIteratorNotNoexcept) {
  static_assert(
      !noexcept(std::is_sorted(
          std::declval<testing::throwing_forward_iterator<local_with_reverse_ordering>>(),
          std::declval<testing::throwing_forward_iterator<local_with_reverse_ordering>>()
      )),
      "Must not be noexcept"
  );
  static_assert(
      !noexcept(std::is_sorted(
          std::declval<testing::throwing_forward_iterator<local_with_reverse_ordering>>(),
          std::declval<testing::throwing_forward_iterator<local_with_reverse_ordering>>(),
          std::declval<non_throwing_comparison>()
      )),
      "Must not be noexcept"
  );
}

/// @test @c is_sorted is not @c noexcept if comparison is not @c noexcept
TEST(IsSorted, IsNotNoexceptIfComparisonNotNoexcept) {
  static_assert(
      !noexcept(std::is_sorted(
          std::declval<testing::non_throwing_forward_iterator<type_with_throwing_comparison>>(),
          std::declval<testing::non_throwing_forward_iterator<type_with_throwing_comparison>>()
      )),
      "Must not be noexcept"
  );
  static_assert(
      !noexcept(std::is_sorted(
          std::declval<testing::non_throwing_forward_iterator<local_with_reverse_ordering>>(),
          std::declval<testing::non_throwing_forward_iterator<local_with_reverse_ordering>>(),
          std::declval<throwing_comparison>()
      )),
      "Must not be noexcept"
  );
}

/// @test @c is_sorted stops when it reaches an out-of-order element
TEST(IsSorted, OnlyAppliesComparisonUpToOutOfOrderElement) {
  std::initializer_list<std::uint32_t> const values{1, 2, 10, 10, 20, 15, 19, 21, 50};

  struct counted_less_than {
    std::uint32_t& counter;

    auto operator()(std::uint32_t lhs, std::uint32_t rhs) -> bool {
      ++counter;
      return lhs < rhs;
    }
  };

  std::uint32_t counter{0};

  ASSERT_TRUE(std::is_sorted(std::begin(values), std::begin(values) + 5, counted_less_than{counter}));
  ASSERT_EQ(counter, 4);
  counter = 0;
  ASSERT_FALSE(std::is_sorted(std::begin(values), std::end(values), counted_less_than{counter}));
  ASSERT_EQ(counter, 5);
}

template <typename T>
class IsSortedIteratorTests : public testing::Test {};

using iterator_tag_types =
    ::testing::Types<std::forward_iterator_tag, std::bidirectional_iterator_tag, std::random_access_iterator_tag>;

TYPED_TEST_SUITE(IsSortedIteratorTests, iterator_tag_types, );

/// @test is_sorted returns a boolean
TYPED_TEST(IsSortedIteratorTests, IsSortedTakesAnIteratorPairAndReturnsABoolean) {
  testing::StaticAssertTypeEq<
      bool,
      decltype(std::is_sorted(
          std::declval<testing::demoted_iterator<std::uint32_t*, TypeParam>>(),
          std::declval<testing::demoted_iterator<std::uint32_t*, TypeParam>>()
      ))>();
  testing::StaticAssertTypeEq<
      bool,
      decltype(std::is_sorted(
          std::declval<testing::demoted_iterator<local_with_reverse_ordering*, TypeParam>>(),
          std::declval<testing::demoted_iterator<local_with_reverse_ordering*, TypeParam>>()
      ))>();
}

/// @test If elements are out of order, the range is not sorted
TYPED_TEST(IsSortedIteratorTests, AnOutOfOrderArrayIsNotSorted) {
  using iterator_type = testing::demoted_iterator<std::uint32_t const*, TypeParam>;
  std::initializer_list<std::uint32_t> const data{1, 4, 3, 2};

  ASSERT_FALSE(std::is_sorted(iterator_type{std::begin(data)}, iterator_type{std::end(data)}));
}

/// @test A range of elements in increasing order is sorted
TYPED_TEST(IsSortedIteratorTests, AnInOrderArrayIsSorted) {
  using iterator_type = testing::demoted_iterator<std::uint32_t const*, TypeParam>;
  std::initializer_list<std::uint32_t> const data{10, 20, 35, 99};

  ASSERT_TRUE(std::is_sorted(iterator_type{std::begin(data)}, iterator_type{std::end(data)}));
}

/// @test If elements are out of order, the range is not sorted
TYPED_TEST(IsSortedIteratorTests, AnOutOfOrderUDTArrayIsNotSorted) {
  using iterator_type = testing::demoted_iterator<local_with_reverse_ordering const*, TypeParam>;
  std::initializer_list<local_with_reverse_ordering> const data{
      local_with_reverse_ordering{1},
      local_with_reverse_ordering{4},
      local_with_reverse_ordering{3},
      local_with_reverse_ordering{2}
  };

  ASSERT_FALSE(std::is_sorted(iterator_type{std::begin(data)}, iterator_type{std::end(data)}));
}

/// @test A range of elements in increasing order is sorted
TYPED_TEST(IsSortedIteratorTests, AnInOrderUDTArrayIsSorted) {
  using iterator_type = testing::demoted_iterator<local_with_reverse_ordering const*, TypeParam>;
  std::initializer_list<local_with_reverse_ordering> const data{
      local_with_reverse_ordering{99},
      local_with_reverse_ordering{65},
      local_with_reverse_ordering{35},
      local_with_reverse_ordering{10}
  };

  ASSERT_TRUE(std::is_sorted(iterator_type{std::begin(data)}, iterator_type{std::end(data)}));
}

/// @test is_sorted returns a boolean
TYPED_TEST(IsSortedIteratorTests, IsSortedTakesAnIteratorPairAndComparatorAndReturnsABoolean) {
  testing::StaticAssertTypeEq<
      bool,
      decltype(std::is_sorted(
          std::declval<testing::demoted_iterator<std::uint32_t*, TypeParam>>(),
          std::declval<testing::demoted_iterator<std::uint32_t*, TypeParam>>(),
          std::declval<opposite_sorting>()
      ))>();
  testing::StaticAssertTypeEq<
      bool,
      decltype(std::is_sorted(
          std::declval<testing::demoted_iterator<local_with_reverse_ordering*, TypeParam>>(),
          std::declval<testing::demoted_iterator<local_with_reverse_ordering*, TypeParam>>(),
          std::declval<opposite_sorting>()
      ))>();
}

/// @test If elements are out of order for the comparator, the range is not sorted
TYPED_TEST(IsSortedIteratorTests, AnOutOfOrderArrayIsNotSortedWithComparator) {
  using iterator_type = testing::demoted_iterator<std::uint32_t const*, TypeParam>;
  std::initializer_list<std::uint32_t> const data{1, 4, 3, 2};

  ASSERT_FALSE(std::is_sorted(iterator_type{std::begin(data)}, iterator_type{std::end(data)}, opposite_sorting{}));
}

/// @test A range of elements in order as per the comparator is sorted
TYPED_TEST(IsSortedIteratorTests, AnInOrderArrayIsSortedWithComparator) {
  using iterator_type = testing::demoted_iterator<std::uint32_t const*, TypeParam>;
  std::initializer_list<std::uint32_t> const data{123, 42, 35, 2};

  ASSERT_TRUE(std::is_sorted(iterator_type{std::begin(data)}, iterator_type{std::end(data)}, opposite_sorting{}));
}

/// @test If elements are out of order for the comparator, the range is not sorted
TYPED_TEST(IsSortedIteratorTests, AnOutOfOrderUDTArrayIsNotSortedWithComparator) {
  using iterator_type = testing::demoted_iterator<local_with_reverse_ordering const*, TypeParam>;
  std::initializer_list<local_with_reverse_ordering> const data{
      local_with_reverse_ordering{1},
      local_with_reverse_ordering{4},
      local_with_reverse_ordering{3},
      local_with_reverse_ordering{2}
  };

  ASSERT_FALSE(std::is_sorted(iterator_type{std::begin(data)}, iterator_type{std::end(data)}, opposite_sorting{}));
}

/// @test A range of elements in order per the comparator is sorted
TYPED_TEST(IsSortedIteratorTests, AnInOrderUDTArrayIsSortedWithComparator) {
  using iterator_type = testing::demoted_iterator<local_with_reverse_ordering const*, TypeParam>;
  std::initializer_list<local_with_reverse_ordering> const data{
      local_with_reverse_ordering{10},
      local_with_reverse_ordering{65},
      local_with_reverse_ordering{135},
      local_with_reverse_ordering{1000}
  };

  ASSERT_TRUE(std::is_sorted(iterator_type{std::begin(data)}, iterator_type{std::end(data)}, opposite_sorting{}));
}

}  // namespace
