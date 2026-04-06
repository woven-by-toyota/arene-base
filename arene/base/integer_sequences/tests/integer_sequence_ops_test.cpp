// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/integer_sequences.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using ::arene::base::constraints;
using ::arene::base::integer_sequence_cat;
using ::arene::base::integer_sequence_contains_v;
using ::arene::base::integer_sequence_count_of;
using ::arene::base::integer_sequence_count_of_v;
using ::arene::base::integer_sequence_element;
using ::arene::base::integer_sequence_element_v;
using ::arene::base::integer_sequence_index_of;
using ::arene::base::integer_sequence_index_of_v;
using ::arene::base::integer_sequence_unique_elements;
using ::arene::base::integer_sequence_ops_detail::get_integer_sequence_count_of;
using ::arene::base::integer_sequence_ops_detail::get_integer_sequence_element;
using ::arene::base::integer_sequence_ops_detail::get_integer_sequence_index_of;
using ::arene::base::integer_sequence_ops_detail::get_integer_sequence_index_of_impl;
using ::arene::base::integer_sequence_ops_detail::integer_sequence_contains;

template <typename T, typename = constraints<>>
constexpr bool has_value_member = false;

template <typename T>
constexpr bool has_value_member<T, constraints<decltype(T::value)>> = true;

/// @test `integer_sequence_cat` concatenates multiple integer sequences into one.
TEST(IntegerSequence, ConcatenateIntegerSequences) {
  ::testing::
      StaticAssertTypeEq<integer_sequence_cat<std::index_sequence<10, 1, 35, 99>>, std::index_sequence<10, 1, 35, 99>>(
      );
  ::testing::StaticAssertTypeEq<integer_sequence_cat<std::index_sequence<1, 2, 3, 4>>, std::index_sequence<1, 2, 3, 4>>(
  );
  ::testing::StaticAssertTypeEq<
      integer_sequence_cat<std::index_sequence<1, 2, 3, 4>, std::index_sequence<10, 20, 30>>,
      std::index_sequence<1, 2, 3, 4, 10, 20, 30>>();

  ::testing::StaticAssertTypeEq<
      integer_sequence_cat<std::index_sequence<1, 2, 3, 4>, std::index_sequence<>, std::index_sequence<10, 20, 30>>,
      std::index_sequence<1, 2, 3, 4, 10, 20, 30>>();
  ::testing::StaticAssertTypeEq<
      integer_sequence_cat<
          std::index_sequence<1, 2, 3, 4>,
          std::index_sequence<1, 1, 1>,
          std::index_sequence<10, 20, 30>>,
      std::index_sequence<1, 2, 3, 4, 1, 1, 1, 10, 20, 30>>();
  ::testing::StaticAssertTypeEq<
      integer_sequence_cat<
          std::index_sequence<1, 2, 3, 4>,
          std::index_sequence<1, 1, 1>,
          std::index_sequence<10, 20, 30>,
          std::index_sequence<99, 88, 123, 45>,
          std::index_sequence<1, 2, 5, 6>,
          std::index_sequence<>,
          std::index_sequence<9, 8, 7>,
          std::index_sequence<>,
          std::index_sequence<100, 200, 300, 400, 500>>,
      std::index_sequence<
          1,
          2,
          3,
          4,
          1,
          1,
          1,
          10,
          20,
          30,
          99,
          88,
          123,
          45,
          1,
          2,
          5,
          6,
          9,
          8,
          7,
          100,
          200,
          300,
          400,
          500>>();

  ::testing::StaticAssertTypeEq<
      integer_sequence_cat<
          std::integer_sequence<int, -1, 2, 3, 4>,
          std::integer_sequence<int, -1, 1, 1>,
          std::integer_sequence<int, -10, 20, 30>,
          std::integer_sequence<int, -99, 88, 123, 45>,
          std::integer_sequence<int, -1, 2, 5, 6>,
          std::integer_sequence<int>,
          std::integer_sequence<int, -9, 8, -7>,
          std::integer_sequence<int>,
          std::integer_sequence<int, 100, 200, -300, 400, 500>>,
      std::integer_sequence<
          int,
          -1,
          2,
          3,
          4,
          -1,
          1,
          1,
          -10,
          20,
          30,
          -99,
          88,
          123,
          45,
          -1,
          2,
          5,
          6,
          -9,
          8,
          -7,
          100,
          200,
          -300,
          400,
          500>>();
}

/// @test `get_integer_sequence_element` returns the specified element of the integer sequence.
TEST(GetIntegerSequenceElement, ReturnsTheIndexthElementOfTheIntegerSequence) {
  STATIC_ASSERT_EQ((get_integer_sequence_element<0>(std::integer_sequence<int, 7, 6, 5, 4>{})), 7);
  STATIC_ASSERT_EQ((get_integer_sequence_element<3>(std::integer_sequence<int, 7, 6, 5, 4>{})), 4);
}

/// @test `integer_sequence_element` returns the specified element of the integer sequence.
TEST(IntegerSequence, GetNthElement) {
  STATIC_ASSERT_EQ((integer_sequence_element<0, std::index_sequence<99>>::value), 99);
  STATIC_ASSERT_EQ((integer_sequence_element<0, std::integer_sequence<std::int16_t, -99>>::value), -99);
  STATIC_ASSERT_EQ((integer_sequence_element<0, std::index_sequence<123, 1, 2, 3, 4>>::value), 123);
  STATIC_ASSERT_EQ((integer_sequence_element<1, std::index_sequence<99, 123>>::value), 123);
  STATIC_ASSERT_EQ((integer_sequence_element<1, std::integer_sequence<int, 99, -123>>::value), -123);
  STATIC_ASSERT_EQ((integer_sequence_element<1, std::index_sequence<1, 2, 3, 4>>::value), 2);
  STATIC_ASSERT_EQ((integer_sequence_element_v<0, std::index_sequence<99>>), 99);
  STATIC_ASSERT_EQ((integer_sequence_element_v<0, std::integer_sequence<unsigned char, 99>>), 99);
  STATIC_ASSERT_EQ((integer_sequence_element_v<0, std::index_sequence<123, 1, 2, 3, 4>>), 123);
  STATIC_ASSERT_EQ((integer_sequence_element_v<1, std::index_sequence<99, 123>>), 123);
  STATIC_ASSERT_EQ((integer_sequence_element_v<1, std::index_sequence<1, 2, 3, 4>>), 2);

  STATIC_ASSERT_FALSE((has_value_member<integer_sequence_element<0, std::index_sequence<>>>));
  STATIC_ASSERT_FALSE((has_value_member<integer_sequence_element<1, std::index_sequence<99>>>));
  STATIC_ASSERT_FALSE((has_value_member<integer_sequence_element<100, std::index_sequence<1, 2, 3, 4>>>));
}

/// @test `integer_sequence_contains` returns true if the specified value is in the sequence.
TEST(IntegerSequenceContains, ReturnsTrueIfValueIsInSequence) {
  STATIC_ASSERT_TRUE((integer_sequence_contains(7, std::integer_sequence<int, 7, 6, 5, 4>{})));
  STATIC_ASSERT_TRUE((integer_sequence_contains(4, std::integer_sequence<int, 7, 6, 5, 4>{})));
}

/// @test `integer_sequence_contains` returns false if the specified value is not in the sequence.
TEST(IntegerSequenceContains, ReturnsFalseIfValueNotInSequence) {
  STATIC_ASSERT_FALSE((integer_sequence_contains(0, std::integer_sequence<int, 7, 6, 5, 4>{})));
  STATIC_ASSERT_FALSE((integer_sequence_contains(100, std::integer_sequence<int, 7, 6, 5, 4>{})));
}

/// @test `integer_sequence_contains` always returns false for an empty sequence.
TEST(IntegerSequenceContains, EmptySequenceAlwaysReturnsFalse) {
  STATIC_ASSERT_FALSE((integer_sequence_contains(0, std::integer_sequence<int>{})));
  STATIC_ASSERT_FALSE((integer_sequence_contains(100, std::integer_sequence<int>{})));
}

/// @test `integer_sequence_contains_v` checks if the specified value is in the sequence.
TEST(IntegerSequence, HasValue) {
  STATIC_ASSERT_FALSE((integer_sequence_contains_v<std::index_sequence<>, 0>));
  STATIC_ASSERT_FALSE((integer_sequence_contains_v<std::index_sequence<1, 2, 3>, 0>));
  STATIC_ASSERT_TRUE((integer_sequence_contains_v<std::index_sequence<0>, 0>));
  STATIC_ASSERT_TRUE((integer_sequence_contains_v<std::index_sequence<1, 2, 3>, 1>));
  STATIC_ASSERT_TRUE((integer_sequence_contains_v<std::index_sequence<2, 99, 1>, 1>));
  STATIC_ASSERT_TRUE((integer_sequence_contains_v<std::integer_sequence<int, 1, 2, 3>, 1>));
}

/// @test `get_integer_sequence_index_of_impl` returns the first index of the value that matches.
TEST(GetIntegerSequenceIndexOfImpl, ReturnsFirstIndexOfValueThatMatches) {
  STATIC_ASSERT_EQ((get_integer_sequence_index_of_impl(7, {7, 7, 5, 4})), 0);
  STATIC_ASSERT_EQ((get_integer_sequence_index_of_impl(4, {7, 7, 5, 4})), 3);
}

/// @test `get_integer_sequence_index_of_impl` returns the size if the value is not found.
TEST(GetIntegerSequenceIndexOfImpl, ReturnsSizeIfNotFound) {
  STATIC_ASSERT_EQ((get_integer_sequence_index_of_impl(10, {7, 7, 5, 4})), 4);
  STATIC_ASSERT_EQ((get_integer_sequence_index_of_impl(0, {7, 7, 5, 4})), 4);
}

/// @test `get_integer_sequence_index_of` returns the first index of the value that matches.
TEST(GetIntegerSequenceIndexOf, ReturnsFirstIndexOfValueThatMatches) {
  STATIC_ASSERT_EQ((get_integer_sequence_index_of<int, 7>(std::integer_sequence<int, 7, 7, 5, 4>{})), 0);
  STATIC_ASSERT_EQ((get_integer_sequence_index_of<int, 4>(std::integer_sequence<int, 7, 7, 5, 4>{})), 3);
}

/// @test `integer_sequence_index_of` returns the index of the specified value in the sequence, or false if the
/// specified value is not in the sequence. `integer_sequence_index_of_v` returns the index of the specified value in
/// the sequence.
TEST(IntegerSequence, IndexOf) {
  STATIC_ASSERT_FALSE((has_value_member<arene::base::integer_sequence_index_of<std::index_sequence<>, 123>>));
  STATIC_ASSERT_FALSE(
      (has_value_member<arene::base::integer_sequence_index_of<std::integer_sequence<int, -1, -2, -3>, -123>>)
  );
  STATIC_ASSERT_EQ((integer_sequence_index_of<std::index_sequence<123>, 123>::value), 0);
  STATIC_ASSERT_EQ((integer_sequence_index_of<std::integer_sequence<int, -123, -2, -3>, -123>::value), 0);
  STATIC_ASSERT_EQ((integer_sequence_index_of<std::integer_sequence<int, -3, -123, -2, -3>, -123>::value), 1);
  STATIC_ASSERT_EQ(
      (integer_sequence_index_of<std::integer_sequence<signed char, 0, 10, 20, -3, 1, 45, -3, -3, -3, -3>, -3>::value),
      3
  );

  STATIC_ASSERT_EQ((integer_sequence_index_of_v<std::index_sequence<123>, 123>), 0);
  STATIC_ASSERT_EQ((integer_sequence_index_of_v<std::integer_sequence<int, -123, -2, -3>, -123>), 0);
  STATIC_ASSERT_EQ((integer_sequence_index_of_v<std::integer_sequence<int, -3, -123, -2, -3>, -123>), 1);
  STATIC_ASSERT_EQ(
      (integer_sequence_index_of_v<std::integer_sequence<signed char, 0, 10, 20, -3, 1, 45, -3, -3, -3, -3>, -3>),
      3
  );
}

/// @test `get_integer_sequence_count_of` returns the number of occurrences of the specified value in the sequence.
TEST(GetIntegerSequenceCountOf, ReturnsCountOfElementsInSequence) {
  STATIC_ASSERT_EQ((get_integer_sequence_count_of(7, std::integer_sequence<int, 7, 7, 5, 4>{})), 2);
  STATIC_ASSERT_EQ((get_integer_sequence_count_of(4, std::integer_sequence<int, 7, 7, 5, 4>{})), 1);
}

/// @test `get_integer_sequence_count_of` returns zero if the specified value is not in the sequence.
TEST(GetIntegerSequenceCountOf, ReturnsZeroIfElementNotInSequence) {
  STATIC_ASSERT_EQ((get_integer_sequence_count_of(0, std::integer_sequence<int, 7, 7, 5, 4>{})), 0);
  STATIC_ASSERT_EQ((get_integer_sequence_count_of(100, std::integer_sequence<int, 7, 7, 5, 4>{})), 0);
}

/// @test `get_integer_sequence_count_of` returns zero for an empty sequence.
TEST(GetIntegerSequenceCountOf, ReturnsZeroForEmptySequence) {
  STATIC_ASSERT_EQ((get_integer_sequence_count_of(0, std::integer_sequence<int>{})), 0);
  STATIC_ASSERT_EQ((get_integer_sequence_count_of(100, std::integer_sequence<int>{})), 0);
}

/// @test `integer_sequence_count_of` and `integer_sequence_count_of_v` return the number of occurrences of the
/// specified value in the sequence.
TEST(IntegerSequence, CountOf) {
  STATIC_ASSERT_EQ((integer_sequence_count_of<std::index_sequence<>, 123>::value), 0);
  STATIC_ASSERT_EQ((integer_sequence_count_of<std::integer_sequence<int, -1, -2, -3>, -123>::value), 0);
  STATIC_ASSERT_EQ((integer_sequence_count_of<std::index_sequence<123>, 123>::value), 1);
  STATIC_ASSERT_EQ((integer_sequence_count_of<std::integer_sequence<int, -123, -2, -3>, -123>::value), 1);
  STATIC_ASSERT_EQ((integer_sequence_count_of<std::integer_sequence<int, -3, -123, -2, -3>, -123>::value), 1);
  STATIC_ASSERT_EQ(
      (integer_sequence_count_of<std::integer_sequence<signed char, 0, 10, 20, -3, 1, 45, -3, -3, -3, -3>, -3>::value),
      5
  );

  STATIC_ASSERT_EQ((integer_sequence_count_of_v<std::index_sequence<123>, 123>), 1);
  STATIC_ASSERT_EQ((integer_sequence_count_of_v<std::integer_sequence<int, -123, -2, -3>, -123>), 1);
  STATIC_ASSERT_EQ((integer_sequence_count_of_v<std::integer_sequence<int, -3, -123, -2, -3>, -123>), 1);
  STATIC_ASSERT_EQ(
      (integer_sequence_count_of_v<std::integer_sequence<signed char, 0, 10, 20, -3, 1, 45, -3, -3, -3, -3>, -3>),
      5
  );
}

/// @test `integer_sequence_unique_elements` returns a sequence of unique elements.
TEST(IntegerSequence, UniqueElements) {
  ::testing::StaticAssertTypeEq<
      integer_sequence_unique_elements<std::integer_sequence<int, 1, 1, -2, 3, 1, -2, 3, 3, 4, 1, 1, -2, 1, 3, 4>>,
      std::integer_sequence<int, 1, -2, 3, 4>>();
  ::testing::StaticAssertTypeEq<
      integer_sequence_unique_elements<
          std::integer_sequence<signed char, 1, 1, -2, 3, 1, -2, 3, 3, 4, 1, 1, -2, 1, 3, 4>>,
      std::integer_sequence<signed char, 1, -2, 3, 4>>();
  ::testing::StaticAssertTypeEq<
      integer_sequence_unique_elements<std::integer_sequence<signed char>>,
      std::integer_sequence<signed char>>();
  ::testing::StaticAssertTypeEq<
      integer_sequence_unique_elements<std::integer_sequence<unsigned, 42>>,
      std::integer_sequence<unsigned, 42>>();
}

}  // namespace
