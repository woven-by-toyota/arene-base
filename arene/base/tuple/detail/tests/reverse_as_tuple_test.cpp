// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/diagnostics.hpp"

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "No arithmetic is performed on these values so float equality is OK");

#include <gtest/gtest.h>

#include "arene/base/array/array.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/make_tuple.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/pair.hpp"
#include "arene/base/stdlib_choice/tuple.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/tuple/detail/reverse_as_tuple.hpp"
#include "testlibs/utilities/configurable_value.hpp"
#include "testlibs/utilities/counting_wrapper.hpp"

namespace {

using ::arene::base::tuple_detail::reverse_as_tuple;

/// @test reversing an empty tuple yields an empty tuple
CONSTEXPR_TEST(ReverseAsTuple, EmptyTupleYieldsEmptyTuple) {
  auto const result = reverse_as_tuple(std::tuple<>{});
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<> const>();
  CONSTEXPR_ASSERT_EQ(result, std::tuple<>{});
}

/// @test reversing a single-element tuple yields the same tuple
CONSTEXPR_TEST(ReverseAsTuple, SingleElementIsIdentity) {
  auto const result = reverse_as_tuple(std::tuple<int>{42});
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<int> const>();
  CONSTEXPR_ASSERT_EQ(result, std::make_tuple(42));
}

/// @test reversing a two-element tuple swaps the elements
CONSTEXPR_TEST(ReverseAsTuple, TwoElementsAreSwapped) {
  auto const result = reverse_as_tuple(std::tuple<int, double>{1, 2.0});
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<double, int> const>();
  CONSTEXPR_ASSERT_EQ(result, std::make_tuple(2.0, 1));
}

/// @test reversing a three-element tuple reverses element order
CONSTEXPR_TEST(ReverseAsTuple, ThreeElementsAreReversed) {
  auto const result = reverse_as_tuple(std::make_tuple('a', 1, 2.0));
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<double, int, char> const>();
  CONSTEXPR_ASSERT_EQ(result, std::make_tuple(2.0, 1, 'a'));
}

/// @test reversing twice yields the original values
CONSTEXPR_TEST(ReverseAsTuple, ReversingTwiceIsIdentity) {
  auto const original = std::make_tuple(1, 2.0, 'a');
  auto const result = reverse_as_tuple(reverse_as_tuple(original));
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<int, double, char> const>();
  CONSTEXPR_ASSERT_EQ(result, original);
}

/// @test reverse works with an array, returning a std::tuple
CONSTEXPR_TEST(ReverseAsTuple, RvalueArrayReturnsTuple) {
  auto const result = reverse_as_tuple(arene::base::array<int, 3>{1, 2, 3});
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<int, int, int> const>();
  CONSTEXPR_ASSERT_EQ(result, std::make_tuple(3, 2, 1));
}

/// @test reverse works with a pair, returning a std::tuple
CONSTEXPR_TEST(ReverseAsTuple, RvaluePairReturnsTuple) {
  auto const result = reverse_as_tuple(std::pair<int, double>{1, 2.0});
  ::testing::StaticAssertTypeEq<decltype(result), std::tuple<double, int> const>();
  CONSTEXPR_ASSERT_EQ(result, std::make_tuple(2.0, 1));
}

using counted = ::testing::counting_wrapper<int>;

/// @test reversing an rvalue tuple moves elements, does not copy
TEST(ReverseAsTupleValueCategory, RvalueTupleMovesElements) {
  counted::count = {};
  auto source = std::tuple<counted, counted>{counted{1}, counted{2}};
  counted::count = {};

  auto const result = reverse_as_tuple(std::move(source));

  EXPECT_EQ(result, (std::tuple<counted, counted>{counted{2}, counted{1}}));
  ASSERT_GT(counted::count.move_ctor, 0U);
  EXPECT_EQ(counted::count.copy_ctor, 0U);
}

/// @test reversing a const lvalue tuple copies elements, does not move
TEST(ReverseAsTupleValueCategory, LvalueTupleCopiesElements) {
  counted::count = {};
  auto const source = std::tuple<counted, counted>{counted{1}, counted{2}};
  counted::count = {};

  auto const result = reverse_as_tuple(source);

  EXPECT_EQ(result, (std::tuple<counted, counted>{counted{2}, counted{1}}));
  ASSERT_GT(counted::count.copy_ctor, 0U);
}

/// @test reversing an rvalue array moves elements, does not copy
TEST(ReverseAsTupleValueCategory, RvalueArrayMovesElements) {
  counted::count = {};
  auto source = arene::base::array<counted, 2>{counted{1}, counted{2}};
  counted::count = {};

  auto const result = reverse_as_tuple(std::move(source));

  EXPECT_EQ(result, std::make_tuple(counted{2}, counted{1}));
  ASSERT_GT(counted::count.move_ctor, 0U);
  EXPECT_EQ(counted::count.copy_ctor, 0U);
}

/// @test reversing a const lvalue array copies elements, does not move
TEST(ReverseAsTupleValueCategory, LvalueArrayCopiesElements) {
  counted::count = {};
  auto const source = arene::base::array<counted, 2>{counted{1}, counted{2}};
  counted::count = {};

  auto const result = reverse_as_tuple(source);

  EXPECT_EQ(result, std::make_tuple(counted{2}, counted{1}));
  ASSERT_GT(counted::count.copy_ctor, 0U);
}

/// @test reference-qualified element types are preserved in the reversed
/// tuple: reversing a @c std::tuple<int&,double&> yields
/// @c std::tuple<double&,int&> that still aliases the originals
TEST(ReverseAsTupleValueCategory, LvalueReferenceElementsArePreserved) {
  auto first = 1;
  auto second = 2.0;
  auto source = std::tuple<int&, double&>{first, second};

  auto reversed = reverse_as_tuple(source);

  ::testing::StaticAssertTypeEq<decltype(reversed), std::tuple<double&, int&>>();

  // mutating through the reversed tuple must affect the original variables,
  // proving element types are reference types aliasing 'first' and 'second'
  std::get<0>(reversed) = 20.0;
  std::get<1>(reversed) = 10;
  EXPECT_EQ(first, 10);
  EXPECT_EQ(second, 20.0);
}

/// @test reference-qualified element types are preserved for an rvalue input
/// tuple: @c std::get on an xvalue tuple preserves the original element
/// reference category via reference collapsing, and @c reverse_as_tuple must not
/// decay those qualifiers away
TEST(ReverseAsTupleValueCategory, ReferenceElementTypesArePreservedForRvalueInput) {
  static_assert(
      std::is_same<
          decltype(reverse_as_tuple(std::declval<std::tuple<int&, double&&, char const&&>&&>())),
          std::tuple<char const&&, double&&, int&>>::value,
      "reference qualifiers on element types must be preserved"
  );
}

/// @test reversing an empty tuple is noexcept
TEST(ReverseAsTupleNoexcept, EmptyTupleIsNoexcept) {
  static_assert(noexcept(reverse_as_tuple(std::declval<std::tuple<>>())), "must be noexcept");
}

/// @test reversing a tuple of built-in types is noexcept if the tuple
/// constructor is noexcept
TEST(ReverseAsTupleNoexcept, TupleOfBuiltinsMatchesConstructorNoexcept) {
  static constexpr auto tuple_constructor_is_noexcept =
      std::is_nothrow_constructible<std::tuple<double, int>, double, int>::value;

  static_assert(
      tuple_constructor_is_noexcept == noexcept(reverse_as_tuple(std::declval<std::tuple<int, double>&&>())),
      "rvalue noexcept must match tuple constructor"
  );
  static_assert(
      tuple_constructor_is_noexcept == noexcept(reverse_as_tuple(std::declval<std::tuple<int, double> const&>())),
      "lvalue noexcept must match tuple constructor"
  );
}

/// @test reversing a pair of built-in types is noexcept if the resulting
/// tuple constructor is noexcept
TEST(ReverseAsTupleNoexcept, PairOfBuiltinsMatchesConstructorNoexcept) {
  static constexpr auto tuple_constructor_is_noexcept =
      std::is_nothrow_constructible<std::tuple<double, int>, double, int>::value;

  static_assert(
      tuple_constructor_is_noexcept == noexcept(reverse_as_tuple(std::declval<std::pair<int, double>&&>())),
      "rvalue noexcept must match tuple constructor"
  );
  static_assert(
      tuple_constructor_is_noexcept == noexcept(reverse_as_tuple(std::declval<std::pair<int, double> const&>())),
      "lvalue noexcept must match tuple constructor"
  );
}

using throwing_copy = ::testing::configurable_value<int, ::testing::throws_on::copy_construct>;

/// @test reversing a tuple with a throwing copy is not noexcept for const
/// lvalue
TEST(ReverseAsTupleNoexcept, TupleThrowingCopyLvalueIsNotNoexcept) {
  static_assert(
      !noexcept(reverse_as_tuple(std::declval<std::tuple<throwing_copy> const&>())),
      "must be noexcept(false) for const lvalue with throwing copy"
  );
}

/// @test reversing a tuple with a throwing copy is noexcept for rvalue if
/// the tuple constructor from rvalues is noexcept
TEST(ReverseAsTupleNoexcept, TupleThrowingCopyRvalueMatchesMoveNoexcept) {
  static constexpr auto tuple_move_constructor_is_noexcept =
      std::is_nothrow_constructible<std::tuple<throwing_copy>, throwing_copy&&>::value;

  static_assert(
      tuple_move_constructor_is_noexcept == noexcept(reverse_as_tuple(std::declval<std::tuple<throwing_copy>&&>())),
      "rvalue noexcept must match tuple move constructor"
  );
}

}  // namespace

ARENE_IGNORE_END();
