// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_TESTS_TUPLE_CAT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_TESTS_TUPLE_CAT_HPP_

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/forward_as_tuple.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple/tests/tuple_cat_fwd.hpp"
#include "arene/base/tuple/tuple_cat.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "testlibs/utilities/configurable_value.hpp"

namespace tuple_cat_test {

/// @brief obtain the tuple_cat function object being tested
/// @tparam Override type used to select the configuration
/// @return reference to the function object under test
template <class Override = config::override>
constexpr auto tuple_cat_fn() noexcept -> auto& {
  return config::function_under_test<Override>;
}

// CONSTEXPR_TEST defines a static variable, but this header is only used in once per test binary, so there is no ODR
// violation.
// NOLINTBEGIN(misc-definitions-in-headers)

/// @test tuple_cat with zero arguments yields an empty tuple
CONSTEXPR_TEST(TupleCat, ZeroArgumentsYieldsEmptyTuple) {
  auto const result = tuple_cat_fn()();
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<> const>();
  CONSTEXPR_ASSERT_EQ(std::tuple_size<decltype(result)>::value, 0U);
}

/// @test tuple_cat of a single tuple preserves its element types
CONSTEXPR_TEST(TupleCat, SingleTuplePassThrough) {
  auto const result = tuple_cat_fn()(std::tuple<int, int>{1, 2});
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<int, int> const>();
  CONSTEXPR_ASSERT_EQ(std::tuple_size<decltype(result)>::value, 2U);
  CONSTEXPR_ASSERT_EQ(std::get<0>(result), 1);
  CONSTEXPR_ASSERT_EQ(std::get<1>(result), 2);
}

/// @test tuple_cat joins two tuples end-to-end
CONSTEXPR_TEST(TupleCat, TwoTuplesConcatenatedInOrder) {
  auto const result = tuple_cat_fn()(std::tuple<int, int>{1, 2}, std::tuple<int>{3});
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<int, int, int> const>();
  CONSTEXPR_ASSERT_EQ(std::get<0>(result), 1);
  CONSTEXPR_ASSERT_EQ(std::get<1>(result), 2);
  CONSTEXPR_ASSERT_EQ(std::get<2>(result), 3);
}

/// @test tuple_cat supports heterogeneous element types
CONSTEXPR_TEST(TupleCat, HeterogeneousElementTypes) {
  auto const result = tuple_cat_fn()(std::tuple<int, char>{1, 'a'}, std::tuple<int, char>{2, 'b'});
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<int, char, int, char> const>();
  CONSTEXPR_ASSERT_EQ(std::get<0>(result), 1);
  CONSTEXPR_ASSERT_EQ(std::get<1>(result), 'a');
  CONSTEXPR_ASSERT_EQ(std::get<2>(result), 2);
  CONSTEXPR_ASSERT_EQ(std::get<3>(result), 'b');
}

/// @test tuple_cat with empty tuples in the pack
CONSTEXPR_TEST(TupleCat, EmptyTuplesInTheMiddle) {
  auto const result = tuple_cat_fn()(std::tuple<int>{1}, std::tuple<>{}, std::tuple<int>{2}, std::tuple<>{});
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<int, int> const>();
  CONSTEXPR_ASSERT_EQ(std::get<0>(result), 1);
  CONSTEXPR_ASSERT_EQ(std::get<1>(result), 2);
}

/// @test tuple_cat concatenates three tuples
CONSTEXPR_TEST(TupleCat, ThreeTuplesConcatenated) {
  auto const result = tuple_cat_fn()(std::tuple<int>{1}, std::tuple<int, int>{2, 3}, std::tuple<int>{4});
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<int, int, int, int> const>();
  CONSTEXPR_ASSERT_EQ(std::get<0>(result), 1);
  CONSTEXPR_ASSERT_EQ(std::get<1>(result), 2);
  CONSTEXPR_ASSERT_EQ(std::get<2>(result), 3);
  CONSTEXPR_ASSERT_EQ(std::get<3>(result), 4);
}

/// @test tuple_cat accepts std::pair and std::tuple together
CONSTEXPR_TEST(TupleCat, MixedStdTupleLikeInputs) {
  auto const result = tuple_cat_fn()(std::pair<int, int>{1, 2}, std::tuple<int>{3});
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<int, int, int> const>();
  CONSTEXPR_ASSERT_EQ(std::get<0>(result), 1);
  CONSTEXPR_ASSERT_EQ(std::get<1>(result), 2);
  CONSTEXPR_ASSERT_EQ(std::get<2>(result), 3);
}

/// @test tuple_cat preserves reference element types from forward_as_tuple inputs
TEST(TupleCat, ForwardAsTupleReferencePreservation) {
  int first = 1;
  int second = 2;
  auto result = tuple_cat_fn()(std::forward_as_tuple(first), std::forward_as_tuple(second));
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<int&, int&>>();
  EXPECT_EQ(&std::get<0>(result), &first);
  EXPECT_EQ(&std::get<1>(result), &second);
  first = 10;
  EXPECT_EQ(std::get<0>(result), 10);
}

/// @test tuple_cat moves rvalue elements into the result
TEST(TupleCat, RValueInputsAreMoved) {
  using move_only = ::testing::configurable_value<
      int,
      ::testing::throws_on::nothing,
      ::testing::disable::copy_construct | ::testing::disable::copy_assign>;

  auto result = tuple_cat_fn()(std::tuple<move_only>{move_only{1}}, std::tuple<move_only>{move_only{2}});
  EXPECT_EQ(std::get<0>(result).value, 1);
  EXPECT_EQ(std::get<1>(result).value, 2);
}

/// @test tuple_cat is SFINAE friendly (not invocable on non-tuple-ish arguments)
TEST(TupleCat, IsSfinaeFriendly) {
  static_assert(
      !arene::base::is_invocable_v<decltype(tuple_cat_fn()), int>,
      "tuple_cat must not be invocable with a non-tuple-ish argument"
  );
  static_assert(
      arene::base::is_invocable_v<decltype(tuple_cat_fn()), std::tuple<int>>,
      "tuple_cat must be invocable with a tuple"
  );
  static_assert(
      arene::base::is_invocable_v<decltype(tuple_cat_fn())>,
      "tuple_cat must be invocable with zero arguments"
  );
}

// NOLINTEND(misc-definitions-in-headers)

}  // namespace tuple_cat_test

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TUPLE_TESTS_TUPLE_CAT_HPP_
