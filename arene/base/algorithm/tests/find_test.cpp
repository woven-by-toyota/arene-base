// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/find.hpp"

#include <cstdint>
#include <sstream>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "arene/base/algorithm/tests/test_iterators.hpp"
#include "arene/base/array.hpp"
#include "arene/base/compare.hpp"
#include "arene/base/detail/wrapped_iterator.hpp"
#include "arene/base/iterator.hpp"

namespace {
/// @test `find` in an empty vector returns the past-the-end iterator.
TEST(Find, CannotFindSomethingInEmptyVector) {
  std::vector<int> vec;
  ASSERT_EQ(arene::base::find(vec.begin(), vec.end(), 42), vec.end());
}

/// @test `find` finds the first element in a vector by value.
TEST(Find, CanFindFirstElement) {
  constexpr auto value = 42;
  std::vector<int> vec = {value};
  ASSERT_EQ(arene::base::find(vec.begin(), vec.end(), value), vec.begin());
}

/// @test `find` returns past-the-end iterator if value is not found.
TEST(Find, ReturnEndIfElementNotThere) {
  constexpr auto value = 42;
  std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8};
  ASSERT_EQ(arene::base::find(vec.begin(), vec.end(), value), vec.end());
}

/// @test `find` returns iterator to found element.
TEST(Find, ReturnIteratorOfFoundElement) {
  constexpr auto value = 42;
  std::vector<int> vec = {1, 2, 3, value, 5, 6, 7, 8};
  ASSERT_EQ(arene::base::find(vec.begin(), vec.end(), value), vec.begin() + 3);
}

/// @test `find` an element in a stream iterator results in the iterator to that element.
TEST(Find, FindInStreamIterator) {
  constexpr auto value = 42;
  auto stream = std::stringstream();
  stream << 1 << " " << 2 << " " << 3 << " " << value << " " << 5 << " " << 6 << " " << 7 << " " << 8 << " "
         << std::endl;
  auto found = arene::base::find(std::istream_iterator<int>(stream), std::istream_iterator<int>(), value);

  ASSERT_EQ(*found, value);
}

/// @test `find` fails to find an element that does not exist in a stream traversed by a stream iterator.
TEST(Find, FailToFindInStreamIterator) {
  constexpr auto value = 42;
  auto stream = std::stringstream();
  stream << 1 << " " << 2 << " " << 3 << " " << 5 << " " << 6 << " " << 7 << " " << 8 << " " << std::endl;
  auto found = arene::base::find(std::istream_iterator<int>(stream), std::istream_iterator<int>(), value);

  ASSERT_EQ(found, std::istream_iterator<int>());
}

/// @test `find` can find a expected value in a constexpr context.
TEST(Find, CanFindInConstexprContext) {
  constexpr std::uint32_t value = 123;
  constexpr std::uint32_t missing = 456;
  static constexpr arene::base::array<std::uint32_t, 8> values{{1, 2, 3, value, 5, 6, 7, 8}};

  static_assert(
      arene::base::find(values.begin(), values.end(), value) == arene::base::next(values.begin(), 3),
      "Can find value"
  );
  static_assert(arene::base::find(values.begin(), values.end(), missing) == values.end(), "Return end if not found");
}

/// @test `find` finds the first occurrence of the element when duplicate elements exist in a collection.
TEST(Find, FindsFirstMatchIfMultiple) {
  constexpr auto value = 42;
  std::vector<int> vec = {1, 2, 3, value, 5, value, 7, value};
  ASSERT_EQ(arene::base::find(vec.begin(), vec.end(), value), vec.begin() + 3);
}

/*
 * Tests for noexcept conditions
 */
/// @test Verify `find` is noexcept if all operators of the iterator argument are noexcept.
TEST(Find, IsNoexceptIfAllNoexcept) {
  static_assert(noexcept(arene::base::find(noexcept_int_iterator(), noexcept_int_iterator(), 42)), "Must be noexcept");
}

/// @test Verify `find` is not noexcept if the increment operator of the iterator argument throws.
TEST(Find, IsNotNoexceptIfIncrementThrows) {
  static_assert(
      !noexcept(arene::base::find(throwing_increment_int_iterator(), throwing_increment_int_iterator(), 42)),
      "Must not be noexcept"
  );
}

/// @test Verify `find` is not noexcept if the comparison operator of the iterator argument throws.
TEST(Find, IsNotNoexceptIfComparisonThrows) {
  static_assert(
      !noexcept(arene::base::find(throwing_comparison_int_iterator(), throwing_comparison_int_iterator(), 42)),
      "Must not be noexcept"
  );
}

/// @test Verify `find` is not noexcept if the reference operator of the iterator argument throws.
TEST(Find, IsNotNoexceptIfReferenceThrows) {
  static_assert(
      !noexcept(arene::base::find(throwing_reference_int_iterator(), throwing_reference_int_iterator(), 42)),
      "Must not be noexcept"
  );
}

/// @test Verify `find` is not noexcept if all operators of the iterator argument throw.
TEST(Find, IsNotNoexceptIfAllThrows) {
  static_assert(
      !noexcept(arene::base::find(throwing_int_iterator(), throwing_int_iterator(), 42)),
      "Must not be noexcept"
  );
}
}  // namespace
