// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iterator_types.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/cstdint"
#include "stdlib/include/initializer_list"
#include "stdlib/include/iterator"
#include "testlibs/minitest/minitest.hpp"
#include "testlibs/utilities/iterator_types.hpp"

namespace {

struct is_non_zero {
  constexpr auto operator()(std::uint32_t val) const noexcept -> bool { return val != 0U; }
};

struct throwing_is_non_zero {
  constexpr auto operator()(std::uint32_t val) const noexcept(false) -> bool { return val != 0U; }
};

/// @test An empty range returns the start iterator
TEST(FindIfNot, EmptyRangeReturnsIterator) {
  std::uint32_t const dummy{0U};

  auto const* const result = std::find_if_not(&dummy, &dummy, is_non_zero{});

  ASSERT_EQ(result, &dummy);
}

/// @test a range where the predicate always returns @c true yields the end of the range
TEST(FindIfNot, RangeWhereEverythingPassesReturnsEndOfRange) {
  std::initializer_list<std::uint32_t> values{1, 1, 1, 1, 1};
  auto const* const result = std::find_if_not(std::begin(values), std::end(values), is_non_zero{});

  ASSERT_EQ(result, std::end(values));
}

/// @test a range where the predicate always returns @c false yields the start of the range
TEST(FindIfNot, RangeWhereNothingPassesReturnsStartOfRange) {
  std::initializer_list<std::uint32_t> values{0, 0, 0, 0, 0};

  auto const* const result = std::find_if_not(std::begin(values), std::end(values), is_non_zero{});

  ASSERT_EQ(result, std::begin(values));
}

/// @test The result is the first position in the range that does not pass the predicate
TEST(FindIfNot, ReturnsFirstEntryThatDoesNotPass) {
  std::initializer_list<std::uint32_t> values{1, 2, 0, 4, 100};

  auto const* const result = std::find_if_not(std::begin(values), std::end(values), is_non_zero{});

  ASSERT_EQ(result, values.begin() + 2);
}

/// @test @c find_if_not is @c noexcept if all operations are @c noexcept
TEST(FindIfNot, NoexceptIfAllOperationsNoexcept) {
  static_assert(
      noexcept(std::find_if_not(
          std::declval<std::uint32_t const*>(),
          std::declval<std::uint32_t const*>(),
          std::declval<is_non_zero>()
      )),
      "Must be noexcept"
  );
}

/// @test @c find_if_not is not @c noexcept if the iterator operations can throw
TEST(FindIfNot, NotNoexceptIfIteratorOperationsNotNoexcept) {
  static_assert(
      !noexcept(std::find_if_not(
          std::declval<testing::throwing_forward_iterator<std::uint32_t>>(),
          std::declval<testing::throwing_forward_iterator<std::uint32_t>>(),
          std::declval<is_non_zero>()
      )),
      "Must not be noexcept"
  );
}

/// @test @c find_if_not is not @c noexcept if the predicate can throw
TEST(FindIfNot, NotNoexceptIfPredicateNotNoexcept) {
  static_assert(
      !noexcept(std::find_if_not(
          std::declval<testing::non_throwing_forward_iterator<std::uint32_t>>(),
          std::declval<testing::non_throwing_forward_iterator<std::uint32_t>>(),
          std::declval<throwing_is_non_zero>()
      )),
      "Must not be noexcept"
  );
}

template <typename T>
class FindIfNotIteratorTests : public testing::Test {};

using iterator_tag_types = ::testing::Types<
    std::input_iterator_tag,
    std::forward_iterator_tag,
    std::bidirectional_iterator_tag,
    std::random_access_iterator_tag>;

TYPED_TEST_SUITE(FindIfNotIteratorTests, iterator_tag_types, );

/// @test a range where the predicate always returns @c true yields the end of the range
TYPED_TEST(FindIfNotIteratorTests, RangeWhereEverythingPassesReturnsEndOfRange) {
  using iterator_type = testing::demoted_iterator<std::uint32_t const*, TypeParam>;
  std::initializer_list<std::uint32_t> values{1, 1, 1, 1, 1};
  iterator_type const result =
      std::find_if_not(iterator_type{std::begin(values)}, iterator_type{std::end(values)}, is_non_zero{});

  ASSERT_EQ(result, iterator_type{std::end(values)});
}

/// @test The result is the first position in the range that does not pass the predicate
TYPED_TEST(FindIfNotIteratorTests, ReturnsFirstEntryThatDoesNotPass) {
  using iterator_type = testing::demoted_iterator<std::uint32_t const*, TypeParam>;
  std::initializer_list<std::uint32_t> values{1, 2, 0, 4, 100};

  iterator_type const result =
      std::find_if_not(iterator_type{std::begin(values)}, iterator_type{std::end(values)}, is_non_zero{});

  ASSERT_EQ(result, iterator_type{values.begin() + 2});
}

}  // namespace
