// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/algorithm/any_of.hpp"

#include <gtest/gtest.h>

#include "arene/base/algorithm/tests/any_all_test_utilities.hpp"
#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/testing/gtest.hpp"

// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) Using iterators that happen to be pointers.

namespace {
/// @test Test `any_of` with no arguments or an empty set is false.
CONSTEXPR_TEST(AnyOf, OfNothingIsFalse) {
  CONSTEXPR_ASSERT(!arene::base::any_of());
  std::initializer_list<bool> const values{};
  CONSTEXPR_ASSERT(!arene::base::any_of(values.begin(), values.end()));
}

/// @test Test `any_of` with one argument has the boolean result of the argument.
CONSTEXPR_TEST(AnyOf, OneElementIsThatElement) {
  CONSTEXPR_ASSERT(!arene::base::any_of(false));
  CONSTEXPR_ASSERT(arene::base::any_of(true));
  CONSTEXPR_ASSERT(!arene::base::any_of(false));
  std::initializer_list<bool> const values{true, false, true};
  CONSTEXPR_ASSERT(arene::base::any_of(values.begin(), values.begin() + 1));
  CONSTEXPR_ASSERT(!arene::base::any_of(values.begin() + 1, values.begin() + 2));
}

/// @test Test `any_of` with multiple arguments is true if all are true.
CONSTEXPR_TEST(AnyOf, ASeriesOfElementsIsTrueIfAllTrue) {
  CONSTEXPR_ASSERT(arene::base::any_of(true, true, true, true));

  std::initializer_list<bool> const values{true, true, true, true, true, true, true, true, true, true};
  CONSTEXPR_ASSERT(arene::base::any_of(values.begin(), values.end()));
}

/// @test Test `any_of` with multiple arguments is false if all are false.
CONSTEXPR_TEST(AnyOf, ASeriesOfElementsIsFalseIfAllFalse) {
  CONSTEXPR_ASSERT(!arene::base::any_of(false, false, false, false));
  std::initializer_list<bool> const values{false, false, false, false, false, false, false, false, false, false};
  CONSTEXPR_ASSERT(!arene::base::any_of(values.begin(), values.end()));
}

/// @test Test `any_of` with multiple arguments is true if any are true.
CONSTEXPR_TEST(AnyOf, ASeriesOfElementsIsTrueIfAnyTrue) {
  CONSTEXPR_ASSERT(arene::base::any_of(true, false, false, false, false));
  CONSTEXPR_ASSERT(arene::base::any_of(false, true, false, false, false));
  CONSTEXPR_ASSERT(arene::base::any_of(false, false, true, false, false));
  CONSTEXPR_ASSERT(arene::base::any_of(false, false, false, true, false));
  CONSTEXPR_ASSERT(arene::base::any_of(false, false, false, false, true));

  std::initializer_list<bool> const values1{true, false, false, false, false, false};
  CONSTEXPR_ASSERT(arene::base::any_of(values1.begin(), values1.end()));
  std::initializer_list<bool> const values2{false, true, false, false, false, false};
  CONSTEXPR_ASSERT(arene::base::any_of(values2.begin(), values2.end()));
  std::initializer_list<bool> const values3{false, false, true, false, false, false};
  CONSTEXPR_ASSERT(arene::base::any_of(values3.begin(), values3.end()));
  std::initializer_list<bool> const values4{false, false, false, true, false, false};
  CONSTEXPR_ASSERT(arene::base::any_of(values4.begin(), values4.end()));
  std::initializer_list<bool> const values5{false, false, false, false, true, false};
  CONSTEXPR_ASSERT(arene::base::any_of(values5.begin(), values5.end()));
}

/// @test Test `any_of` with custom predicate is true if predicate returns true for any element
CONSTEXPR_TEST(AnyOf, WithCustomPredicate) {
  test::assert_result_is(  //
      false,
      arene::base::any_of,
      arene::base::array<int, 0>{},
      test::is_odd{}
  );

  test::assert_result_is(  //
      false,
      arene::base::any_of,
      arene::base::to_array<int>({2, 4, 6, 8, 10}),
      test::is_odd{}
  );

  test::assert_result_is(  //
      true,
      arene::base::any_of,
      arene::base::to_array<int>({1, 4, 6, 8, 10}),
      test::is_odd{}
  );

  test::assert_result_is(  //
      true,
      arene::base::any_of,
      arene::base::to_array<int>({2, 4, 5, 8, 10}),
      test::is_odd{}
  );

  test::assert_result_is(  //
      true,
      arene::base::any_of,
      arene::base::to_array<int>({2, 4, 5, 8, 11}),
      test::is_odd{}
  );

  test::assert_result_is(  //
      true,
      arene::base::any_of,
      arene::base::to_array<int>({1, 3, 5, 7, 9}),
      test::is_odd{}
  );
}

/// @test Test `any_of` with custom predicate and custom projection
CONSTEXPR_TEST(AnyOf, WithCustomPredicateAndProjection) {
  test::assert_result_is(
      false,
      arene::base::any_of,
      arene::base::array<test::wrapped_int, 0>{},
      test::is_odd{},
      test::value_of{}
  );

  test::assert_result_is(
      false,
      arene::base::any_of,
      test::to_array_of<test::wrapped_int>({2, 4, 6, 8, 10}),
      test::is_odd{},
      test::value_of{}
  );

  test::assert_result_is(
      true,
      arene::base::any_of,
      test::to_array_of<test::wrapped_int>({1, 4, 6, 8, 10}),
      test::is_odd{},
      test::value_of{}
  );

  test::assert_result_is(
      true,
      arene::base::any_of,
      test::to_array_of<test::wrapped_int>({2, 4, 5, 8, 10}),
      test::is_odd{},
      test::value_of{}
  );

  test::assert_result_is(
      true,
      arene::base::any_of,
      test::to_array_of<test::wrapped_int>({2, 4, 6, 8, 11}),
      test::is_odd{},
      test::value_of{}
  );

  test::assert_result_is(
      true,
      arene::base::any_of,
      test::to_array_of<test::wrapped_int>({1, 3, 5, 7, 9}),
      test::is_odd{},
      test::value_of{}
  );
}

/// @test Test `any_of` not invocable with invalid predicate
TEST(AnyOf, NotInvocableWithInvalidPredicate) {
  test::static_assert_not_invocable_with_invalid_predicate(arene::base::any_of);
}

/// @test Test `any_of` not invocable with invalid projection
TEST(AnyOf, NotInvocableWithInvalidProjection) {
  test::static_assert_not_invocable_with_invalid_projection(arene::base::any_of);
}

/// @test Test `any_of` not invocable with invalid iterator
TEST(AnyOf, NotInvocableWithInvalidIterator) {
  test::static_assert_not_invocable_with_invalid_iterator(arene::base::any_of);
}

/// @test Test `any_of` has correct noexcept specfication
TEST(AnyOf, NoexceptIsCorrect) { test::static_assert_has_correct_noexcept_specification(arene::base::any_of); }

}  // namespace

// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
