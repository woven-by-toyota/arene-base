// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/find_if.hpp"

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
#include "arene/base/testing/gtest.hpp"

namespace {
template <typename T, T N>
struct equals {
  constexpr auto operator()(T const& value) const -> bool { return value == N; }
};

template <typename T, T N>
struct equals_noexcept {
  constexpr auto operator()(T const& value) const noexcept -> bool { return value == N; }
};

/// @test `find_if` a value in an empty vector returns the past-the-end iterator.
TEST(FindIf, CannotFindSomethingInEmptyVector) {
  std::vector<int> vec;
  ASSERT_EQ(arene::base::find_if(vec.begin(), vec.end(), equals<int, 42>()), vec.end());
}

/// @test `find_if` finds the first element in a vector when the predicate is satisfied.
TEST(FindIf, CanFindFirstElement) {
  constexpr auto value = 42;
  std::vector<int> vec = {value};
  ASSERT_EQ(arene::base::find_if(vec.begin(), vec.end(), equals<int, value>()), vec.begin());
}

/// @test `find_if` returns past-the-end iterator if value is not found.
TEST(FindIf, ReturnEndIfElementNotThere) {
  constexpr auto value = 42;
  std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8};
  ASSERT_EQ(arene::base::find_if(vec.begin(), vec.end(), equals<int, value>()), vec.end());
}

/// @test `find_if` returns iterator to found element.
TEST(FindIf, ReturnIteratorOfFoundElement) {
  constexpr auto value = 42;
  std::vector<int> vec = {1, 2, 3, value, 5, 6, 7, 8};
  ASSERT_EQ(arene::base::find_if(vec.begin(), vec.end(), equals<int, value>()), vec.begin() + 3);
}

/// @test `find_if` an element in a stream iterator results in the iterator to that element.
TEST(FindIf, FindInStreamIterator) {
  constexpr auto value = 42;
  auto stream = std::stringstream();
  stream << 1 << " " << 2 << " " << 3 << " " << value << " " << 5 << " " << 6 << " " << 7 << " " << 8 << " "
         << std::endl;
  auto found =
      arene::base::find_if(std::istream_iterator<int>(stream), std::istream_iterator<int>(), equals<int, value>());

  ASSERT_NE(found, std::istream_iterator<int>());
  ASSERT_EQ(*found, value);

  ++found;
  found = arene::base::find_if(found, std::istream_iterator<int>(), equals<int, value>());

  ASSERT_EQ(found, std::istream_iterator<int>());

  found = arene::base::find_if(std::istream_iterator<int>(), std::istream_iterator<int>(), equals<int, value>());

  ASSERT_EQ(found, std::istream_iterator<int>());
}

/// @test `find_if` can find an element in constexpr context.
TEST(FindIf, CanFindInConstexprContext) {
  constexpr std::uint32_t value = 123;
  static constexpr arene::base::array<std::uint32_t, 8> values{{1, 2, 3, value, 5, 6, 7, 8}};

  STATIC_ASSERT_EQ(
      arene::base::find_if(values.begin(), values.end(), equals<std::uint32_t, value>()),
      arene::base::next(values.begin(), 3)
  );
  STATIC_ASSERT_EQ(
      arene::base::find_if(arene::base::next(values.begin(), 4), values.end(), equals<std::uint32_t, value>()),
      values.end()
  );
  STATIC_ASSERT_EQ(arene::base::find_if(values.end(), values.end(), equals<std::uint32_t, value>()), values.end());
}

/// @test `find_if` only finds the first occurrence of the element.
TEST(FindIf, FindsFirstMatchIfMultiple) {
  constexpr auto value = 42;
  std::vector<int> vec = {1, 2, 3, value, 5, value, 7, value};
  ASSERT_EQ(arene::base::find_if(vec.begin(), vec.end(), equals<int, value>()), vec.begin() + 3);
}

/*
 * Tests for noexcept conditions
 */
/// @test Verify `find_if` is noexcept if all the operators of the iterator argument are noexcept.
TEST(FindIf, IsNoexceptIfAllNoexcept) {
  static_assert(
      noexcept(arene::base::find_if(noexcept_int_iterator(), noexcept_int_iterator(), equals_noexcept<int, 42>())),
      "Must be noexcept"
  );
}

/// @test Verify `find_if` is not noexcept if the increment operator of the iterator argument
/// throws.
TEST(FindIf, IsNotNoexceptIfIncrementThrows) {
  static_assert(
      !noexcept(arene::base::find_if(
          throwing_increment_int_iterator(),
          throwing_increment_int_iterator(),
          equals_noexcept<int, 42>()
      )),
      "Must not be noexcept"
  );
}

/// @test Verify `find_if` is not noexcept if the comparison operator of the iterator argument
/// throws.
TEST(FindIf, IsNotNoexceptIfComparisonThrows) {
  static_assert(
      !noexcept(arene::base::find_if(
          throwing_comparison_int_iterator(),
          throwing_comparison_int_iterator(),
          equals_noexcept<int, 42>()
      )),
      "Must not be noexcept"
  );
}

/// @test Verify `find_if` is not noexcept if the reference operator of the iterator argument
/// throws.
TEST(FindIf, IsNotNoexceptIfReferenceThrows) {
  static_assert(
      !noexcept(arene::base::find_if(
          throwing_reference_int_iterator(),
          throwing_reference_int_iterator(),
          equals_noexcept<int, 42>()
      )),
      "Must not be noexcept"
  );
}

/// @test Verify `find_if` is not noexcept if all of the operators of the iterator argument
/// throw.
TEST(FindIf, IsNotNoexceptIfAllThrows) {
  static_assert(
      !noexcept(arene::base::find_if(throwing_int_iterator(), throwing_int_iterator(), equals_noexcept<int, 42>())),
      "Must not be noexcept"
  );
}
}  // namespace
