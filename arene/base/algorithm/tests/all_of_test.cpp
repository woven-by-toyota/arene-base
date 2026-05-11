// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/all_of.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/tests/any_all_test_utilities.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

/// @test Test `all_of` with no arguments is true.
TEST(AllOf, NothingIsTrue) {
  STATIC_ASSERT_TRUE(arene::base::all_of());
  constexpr auto count = 0;
  constexpr arene::base::array<bool, count> values{};
  STATIC_ASSERT_TRUE(arene::base::all_of(values.begin(), values.end()));
}

/// @test Test `all_of` with one argument has the boolean result of the argument.
TEST(AllOf, OneElementIsThatElement) {
  STATIC_ASSERT_TRUE(arene::base::all_of(true));
  STATIC_ASSERT_FALSE(arene::base::all_of(false));
  constexpr auto count = 3;
  constexpr arene::base::array<bool, count> values{true, false, true};
  STATIC_ASSERT_TRUE(arene::base::all_of(values.begin(), values.begin() + 1));
  STATIC_ASSERT_FALSE(arene::base::all_of(values.begin() + 1, values.begin() + 2));
}

/// @test Test `all_of` with multiple arguments is true if all are true.
TEST(AllOf, ASeriesOfElementsIsTrueIfAllTrue) {
  STATIC_ASSERT_TRUE(arene::base::all_of(true, true, true, true));
  constexpr auto count = 10;
  constexpr arene::base::array<bool, count> values{true, true, true, true, true, true, true, true, true, true};

  STATIC_ASSERT_TRUE(arene::base::all_of(values.begin(), values.end()));
}

/// @test Test `all_of` with multiple arguments is false if any is false.
TEST(AllOf, ASeriesOfElementsIsFalseIfAnyFalse) {
  STATIC_ASSERT_TRUE(!arene::base::all_of(false, true, true, true, true));
  STATIC_ASSERT_TRUE(!arene::base::all_of(true, false, true, true, true));
  STATIC_ASSERT_TRUE(!arene::base::all_of(true, true, false, true, true));
  STATIC_ASSERT_TRUE(!arene::base::all_of(true, true, true, false, true));
  STATIC_ASSERT_TRUE(!arene::base::all_of(true, true, true, true, false));
  constexpr auto count = 6;
  constexpr arene::base::array<bool, count> values1{false, true, true, true, true, true};
  STATIC_ASSERT_TRUE(!arene::base::all_of(values1.begin(), values1.end()));
  constexpr arene::base::array<bool, count> values2{true, false, true, true, true, true};
  STATIC_ASSERT_TRUE(!arene::base::all_of(values2.begin(), values2.end()));
  constexpr arene::base::array<bool, count> values3{true, true, false, true, true, true};
  STATIC_ASSERT_TRUE(!arene::base::all_of(values3.begin(), values3.end()));
  constexpr arene::base::array<bool, count> values4{true, true, true, false, true, true};
  STATIC_ASSERT_TRUE(!arene::base::all_of(values4.begin(), values4.end()));
  constexpr arene::base::array<bool, count> values5{true, true, true, true, false, true};
  STATIC_ASSERT_TRUE(!arene::base::all_of(values5.begin(), values5.end()));
}

/// @test Test `all_of` with custom predicate is true if predicate returns true for all elements
TEST(AllOf, WithCustomPredicate) {
  test::assert_result_is(  //
      true,
      arene::base::all_of,
      arene::base::array<int, 0>{},
      test::is_even{}
  );

  test::assert_result_is(  //
      true,
      arene::base::all_of,
      arene::base::to_array<int>({2, 4, 6, 8, 10}),
      test::is_even{}
  );

  test::assert_result_is(  //
      false,
      arene::base::all_of,
      arene::base::to_array<int>({1, 4, 6, 8, 10}),
      test::is_even{}
  );

  test::assert_result_is(  //
      false,
      arene::base::all_of,
      arene::base::to_array<int>({2, 4, 5, 8, 10}),
      test::is_even{}
  );

  test::assert_result_is(  //
      false,
      arene::base::all_of,
      arene::base::to_array<int>({2, 4, 5, 8, 11}),
      test::is_even{}
  );

  test::assert_result_is(  //
      false,
      arene::base::all_of,
      arene::base::to_array<int>({1, 3, 5, 7, 9}),
      test::is_even{}
  );
}

/// @test Test `all_of` with custom predicate and custom projection
CONSTEXPR_TEST(AllOf, WithCustomPredicateAndProjection) {
  test::assert_result_is(
      true,
      arene::base::all_of,
      arene::base::array<test::wrapped_int, 0>{},
      test::is_even{},
      test::value_of{}
  );

  test::assert_result_is(
      true,
      arene::base::all_of,
      test::to_array_of<test::wrapped_int>({2, 4, 6, 8, 10}),
      test::is_even{},
      test::value_of{}
  );

  test::assert_result_is(
      false,
      arene::base::all_of,
      test::to_array_of<test::wrapped_int>({1, 4, 6, 8, 10}),
      test::is_even{},
      test::value_of{}
  );

  test::assert_result_is(
      false,
      arene::base::all_of,
      test::to_array_of<test::wrapped_int>({2, 4, 5, 8, 10}),
      test::is_even{},
      test::value_of{}
  );

  test::assert_result_is(
      false,
      arene::base::all_of,
      test::to_array_of<test::wrapped_int>({2, 4, 6, 8, 11}),
      test::is_even{},
      test::value_of{}
  );

  test::assert_result_is(
      false,
      arene::base::all_of,
      test::to_array_of<test::wrapped_int>({1, 3, 5, 7, 9}),
      test::is_even{},
      test::value_of{}
  );
}

/// @test Test `all_of` not invocable with invalid predicate
TEST(AllOf, NotInvocableWithInvalidPredicate) {
  test::static_assert_not_invocable_with_invalid_predicate(arene::base::all_of);
}

/// @test Test `all_of` not invocable with invalid projection
TEST(AllOf, NotInvocableWithInvalidProjection) {
  test::static_assert_not_invocable_with_invalid_projection(arene::base::all_of);
}

/// @test Test `all_of` not invocable with invalid iterator
TEST(AllOf, NotInvocableWithInvalidIterator) {
  test::static_assert_not_invocable_with_invalid_iterator(arene::base::all_of);
}

/// @test Test `all_of` has correct noexcept specfication
TEST(AllOf, NoexceptIsCorrect) { test::static_assert_has_correct_noexcept_specification(arene::base::all_of); }

}  // namespace
