// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compare/operators.hpp"

#include <gtest/gtest.h>

#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace {
class convertible_to_int;
}  // namespace

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wdeprecated-declarations", "We still want to test the deprecated classes");

namespace {

using ::arene::base::full_ordering_operators_from_three_way_compare;
using ::arene::base::full_ordering_operators_from_three_way_compare_and_equals;
using ::arene::base::strong_ordering;

struct strong_three_way : full_ordering_operators_from_three_way_compare<strong_three_way> {
  unsigned value;

  constexpr explicit strong_three_way(unsigned that_value) noexcept
      : value(that_value) {}

  static constexpr auto three_way_compare(strong_three_way lhs, strong_three_way rhs) noexcept -> strong_ordering {
    return (lhs.value < rhs.value) ? strong_ordering::less
                                   : ((lhs.value == rhs.value) ? strong_ordering::equal : strong_ordering::greater);
  }
};

struct three_way_with_equality : full_ordering_operators_from_three_way_compare_and_equals<three_way_with_equality> {
  unsigned value;

  constexpr explicit three_way_with_equality(unsigned that_value) noexcept
      : value(that_value) {}

  static constexpr auto three_way_compare(three_way_with_equality lhs, three_way_with_equality rhs) noexcept
      -> strong_ordering {
    return (lhs.value < rhs.value) ? strong_ordering::less
                                   : ((lhs.value == rhs.value) ? strong_ordering::equal : strong_ordering::greater);
  }

  /// Counterintuitive inverse equality to ensure it is used
  friend constexpr auto operator==(three_way_with_equality lhs, three_way_with_equality rhs) noexcept -> bool {
    return lhs.value != rhs.value;
  }
};

struct three_way_with_other : full_ordering_operators_from_three_way_compare<three_way_with_other, unsigned> {
  unsigned value;

  constexpr explicit three_way_with_other(unsigned that_value) noexcept
      : value(that_value) {}

  static constexpr auto three_way_compare(three_way_with_other lhs, unsigned rhs) noexcept -> strong_ordering {
    return (lhs.value < rhs) ? strong_ordering::less
                             : ((lhs.value == rhs) ? strong_ordering::equal : strong_ordering::greater);
  }
};

ARENE_IGNORE_END();

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// the same type has `noexcept` _less than_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareAndBaseClassHasLessThanOperator) {
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<strong_three_way>);
  ::testing::StaticAssertTypeEq<decltype(strong_three_way{1} < strong_three_way{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(strong_three_way{1} < strong_three_way{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// the same type has _less than_ operator returning expected results.
TEST(ThreeWayCompare, LessThanOperatorReflectsResultOfThreeWayComparison) {
  STATIC_ASSERT_LT(strong_three_way{1}, strong_three_way{2});
  STATIC_ASSERT_FALSE((strong_three_way{2} < strong_three_way{1}));
  STATIC_ASSERT_FALSE(strong_three_way{1} < strong_three_way{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// the same type has `noexcept` _greater than_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareAndBaseClassHasGreaterThanOperator) {
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<strong_three_way>);
  ::testing::StaticAssertTypeEq<decltype(strong_three_way{1} > strong_three_way{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(strong_three_way{1} > strong_three_way{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// the same type has _greater than_ operator returning expected results.
TEST(ThreeWayCompare, GreaterThanOperatorReflectsResultOfThreeWayComparison) {
  STATIC_ASSERT_FALSE(strong_three_way{1} > strong_three_way{2});
  STATIC_ASSERT_GT(strong_three_way{2}, strong_three_way{1});
  STATIC_ASSERT_FALSE(strong_three_way{1} > strong_three_way{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// the same type has `noexcept` _equality_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareAndBaseClassHasEqualityOperator) {
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<strong_three_way>);
  ::testing::StaticAssertTypeEq<decltype(strong_three_way{1} == strong_three_way{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(strong_three_way{1} == strong_three_way{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// the same type has _equality_ operator returning expected results.
TEST(ThreeWayCompare, EqualityOperatorReflectsResultOfThreeWayComparison) {
  STATIC_ASSERT_FALSE(strong_three_way{1} == strong_three_way{2});
  STATIC_ASSERT_FALSE(strong_three_way{2} == strong_three_way{1});
  STATIC_ASSERT_EQ(strong_three_way{1}, strong_three_way{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// the same type has `noexcept` _inequality_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareAndBaseClassHasInequalityOperator) {
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<strong_three_way>);
  ::testing::StaticAssertTypeEq<decltype(strong_three_way{1} != strong_three_way{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(strong_three_way{1} != strong_three_way{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// the same type has _inequality_ operator returning expected results.
TEST(ThreeWayCompare, InequalityOperatorReflectsResultOfThreeWayComparison) {
  STATIC_ASSERT_NE(strong_three_way{1}, strong_three_way{2});
  STATIC_ASSERT_NE(strong_three_way{2}, strong_three_way{1});
  STATIC_ASSERT_FALSE(strong_three_way{1} != strong_three_way{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// the same type has `noexcept` _less than or equal_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareAndBaseClassHasLessThanOrEqualOperator) {
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<strong_three_way>);
  ::testing::StaticAssertTypeEq<decltype(strong_three_way{1} <= strong_three_way{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(strong_three_way{1} <= strong_three_way{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// the same type has _less than or equal_ operator returning expected results.
TEST(ThreeWayCompare, LessThanOrEqualOperatorReflectsResultOfThreeWayComparison) {
  STATIC_ASSERT_LE(strong_three_way{1}, strong_three_way{2});
  STATIC_ASSERT_FALSE(strong_three_way{2} <= strong_three_way{1});
  STATIC_ASSERT_LE(strong_three_way{1}, strong_three_way{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// the same type has `noexcept` _greater than or equal_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareAndBaseClassHasGreaterthanOrEqualOperator) {
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<strong_three_way>);
  ::testing::StaticAssertTypeEq<decltype(strong_three_way{1} >= strong_three_way{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(strong_three_way{1} >= strong_three_way{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// the same type has _greater than or equal_ operator returning expected results.
TEST(ThreeWayCompare, GreaterthanOrEqualOperatorReflectsResultOfThreeWayComparison) {
  STATIC_ASSERT_FALSE(strong_three_way{1} >= strong_three_way{2});
  STATIC_ASSERT_GE(strong_three_way{2}, strong_three_way{1});
  STATIC_ASSERT_GE(strong_three_way{1}, strong_three_way{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare_and_equals` has `noexcept` _less than_
/// operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareAndOrderedBaseClassHasLessThanOperator) {
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<three_way_with_equality>);
  ::testing::StaticAssertTypeEq<decltype(three_way_with_equality{1} < three_way_with_equality{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(three_way_with_equality{1} < three_way_with_equality{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare_and_equals` has _less than_ operator
/// returning expected results.
TEST(ThreeWayCompare, OrderedLessThanOperatorReflectsResultOfThreeWayComparison) {
  STATIC_ASSERT_LT(three_way_with_equality{1}, three_way_with_equality{2});
  STATIC_ASSERT_FALSE(three_way_with_equality{2} < three_way_with_equality{1});
  STATIC_ASSERT_FALSE(three_way_with_equality{1} < three_way_with_equality{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare_and_equals` has `noexcept` _greater than_
/// operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareAndOrderedBaseClassHasGreaterThanOperator) {
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<three_way_with_equality>);
  ::testing::StaticAssertTypeEq<decltype(three_way_with_equality{1} > three_way_with_equality{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(three_way_with_equality{1} > three_way_with_equality{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare_and_equals` has _greater than_ operator
/// returning expected results.
TEST(ThreeWayCompare, OrderedGreaterThanOperatorReflectsResultOfThreeWayComparison) {
  STATIC_ASSERT_FALSE(three_way_with_equality{1} > three_way_with_equality{2});
  STATIC_ASSERT_GT(three_way_with_equality{2}, three_way_with_equality{1});
  STATIC_ASSERT_FALSE(three_way_with_equality{1} > three_way_with_equality{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare_and_equals` has `noexcept` _less than or
/// equal_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareAndOrderedBaseClassHasLessThanOrEqualOperator) {
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<three_way_with_equality>);
  ::testing::StaticAssertTypeEq<decltype(three_way_with_equality{1} <= three_way_with_equality{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(three_way_with_equality{1} <= three_way_with_equality{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare_and_equals` has _less than or equal_
/// operator returning expected results.
TEST(ThreeWayCompare, OrderedLessThanOrEqualOperatorReflectsResultOfThreeWayComparison) {
  STATIC_ASSERT_LE(three_way_with_equality{1}, three_way_with_equality{2});
  STATIC_ASSERT_FALSE(three_way_with_equality{2} <= three_way_with_equality{1});
  STATIC_ASSERT_LE(three_way_with_equality{1}, three_way_with_equality{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare_and_equals` has `noexcept` _greater than
/// or equal_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareAndOrderedBaseClassHasGreaterthanOrEqualOperator) {
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<three_way_with_equality>);
  ::testing::StaticAssertTypeEq<decltype(three_way_with_equality{1} >= three_way_with_equality{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(three_way_with_equality{1} >= three_way_with_equality{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare_and_equals` has _greater than or equal_
/// operator returning expected results.
TEST(ThreeWayCompare, OrderedGreaterthanOrEqualOperatorReflectsResultOfThreeWayComparison) {
  STATIC_ASSERT_FALSE(three_way_with_equality{1} >= three_way_with_equality{2});
  STATIC_ASSERT_GE(three_way_with_equality{2}, three_way_with_equality{1});
  STATIC_ASSERT_GE(three_way_with_equality{1}, three_way_with_equality{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare_and_equals` has `noexcept` _equality_
/// operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareAndOrderedBaseClassHasEqualityOperator) {
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<three_way_with_equality>);
  ::testing::StaticAssertTypeEq<decltype(three_way_with_equality{1} == three_way_with_equality{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(three_way_with_equality{1} == three_way_with_equality{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare_and_equals` has _equality_ operator
/// returning expected results.
TEST(ThreeWayCompare, OrderedEqualityOperatorReflectsResultOfThreeWayComparison) {
  STATIC_ASSERT_EQ(three_way_with_equality{1}, three_way_with_equality{2});
  STATIC_ASSERT_EQ(three_way_with_equality{2}, three_way_with_equality{1});
  STATIC_ASSERT_FALSE(three_way_with_equality{1} == three_way_with_equality{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare_and_equals` has `noexcept` _inequality_
/// operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareAndOrderedBaseClassHasInequalityOperator) {
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<three_way_with_equality>);
  ::testing::StaticAssertTypeEq<decltype(three_way_with_equality{1} != three_way_with_equality{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(three_way_with_equality{1} != three_way_with_equality{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare_and_equals` has _inequality_ operator
/// returning expected results.
TEST(ThreeWayCompare, OrderedInequalityOperatorReflectsResultOfThreeWayComparison) {
  STATIC_ASSERT_FALSE(three_way_with_equality{1} != three_way_with_equality{2});
  STATIC_ASSERT_FALSE(three_way_with_equality{2} != three_way_with_equality{1});
  STATIC_ASSERT_NE(three_way_with_equality{1}, three_way_with_equality{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// another type has `noexcept` _less than_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareWithOtherHasLessThanOperator) {
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<three_way_with_other, unsigned>);
  ::testing::StaticAssertTypeEq<decltype(three_way_with_other{1} < 2), bool>();
  STATIC_ASSERT_TRUE(noexcept(three_way_with_other{1} < 2));
  ::testing::StaticAssertTypeEq<decltype(2 < three_way_with_other{1}), bool>();
  STATIC_ASSERT_TRUE(noexcept(2 < three_way_with_other{1}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// another type has `noexcept` _less than_ operator returning expected results.
TEST(ThreeWayCompare, LessThanOperatorReflectsResultOfThreeWayComparisonWithOther) {
  STATIC_ASSERT_LT(three_way_with_other{1}, 2);
  STATIC_ASSERT_FALSE(three_way_with_other{2} < 1);
  STATIC_ASSERT_FALSE(three_way_with_other{1} < 1);

  STATIC_ASSERT_FALSE(2 < three_way_with_other{1});
  STATIC_ASSERT_LT(1, three_way_with_other{2});
  STATIC_ASSERT_FALSE(1 < three_way_with_other{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// another type has `noexcept` _greater than_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareWithOtherHasGreaterThanOperator) {
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<three_way_with_other>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<three_way_with_other, unsigned>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<unsigned, three_way_with_other>);
  ::testing::StaticAssertTypeEq<decltype(three_way_with_other{1} > 2), bool>();
  ::testing::StaticAssertTypeEq<decltype(2 > three_way_with_other{1}), bool>();
  STATIC_ASSERT_TRUE(noexcept(2 > three_way_with_other{1}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// another type has `noexcept` _greater than_ operator returning expected results.
TEST(ThreeWayCompare, GreaterThanOperatorReflectsResultOfThreeWayComparisonWithOther) {
  STATIC_ASSERT_FALSE(three_way_with_other{1} > 2);
  STATIC_ASSERT_GT(three_way_with_other{2}, 1);
  STATIC_ASSERT_FALSE(three_way_with_other{1} > 1);

  STATIC_ASSERT_FALSE(1 > three_way_with_other{2});
  STATIC_ASSERT_GT(2, three_way_with_other{1});
  STATIC_ASSERT_FALSE(1 > three_way_with_other{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// another type has `noexcept` _equality_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareWithOtherHasEqualityOperator) {
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<three_way_with_other>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<three_way_with_other, unsigned>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<unsigned, three_way_with_other>);
  ::testing::StaticAssertTypeEq<decltype(three_way_with_other{1} == 2), bool>();
  ::testing::StaticAssertTypeEq<decltype(1 == three_way_with_other{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(three_way_with_other{1} == 2));
  STATIC_ASSERT_TRUE(noexcept(1 == three_way_with_other{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// another type has `noexcept` _equality_ operator returning expected results.
TEST(ThreeWayCompare, EqualityOperatorReflectsResultOfThreeWayComparisonWithOther) {
  STATIC_ASSERT_FALSE(three_way_with_other{1} == 2);
  STATIC_ASSERT_FALSE(three_way_with_other{2} == 1);
  STATIC_ASSERT_EQ(three_way_with_other{1}, 1);
  STATIC_ASSERT_FALSE(1 == three_way_with_other{2});
  STATIC_ASSERT_FALSE(2 == three_way_with_other{1});
  STATIC_ASSERT_EQ(1, three_way_with_other{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// another type has `noexcept` _inequality_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareWithOtherHasInequalityOperator) {
  STATIC_ASSERT_FALSE(arene::base::is_inequality_comparable_v<three_way_with_other>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<three_way_with_other, unsigned>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<unsigned, three_way_with_other>);
  ::testing::StaticAssertTypeEq<decltype(three_way_with_other{1} != 2), bool>();
  STATIC_ASSERT_TRUE(noexcept(three_way_with_other{1} != 2));
  ::testing::StaticAssertTypeEq<decltype(1 != three_way_with_other{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(1 != three_way_with_other{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// another type has `noexcept` _inequality_ operator returning expected results.
TEST(ThreeWayCompare, InequalityOperatorReflectsResultOfThreeWayComparisonWithOther) {
  STATIC_ASSERT_NE(three_way_with_other{1}, 2);
  STATIC_ASSERT_NE(three_way_with_other{2}, 1);
  STATIC_ASSERT_FALSE(three_way_with_other{1} != 1);
  STATIC_ASSERT_NE(1, three_way_with_other{2});
  STATIC_ASSERT_NE(2, three_way_with_other{1});
  STATIC_ASSERT_FALSE(1 != three_way_with_other{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// another type has `noexcept` _less than or equal_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareWithOtherHasLessThanOrEqualOperator) {
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<three_way_with_other>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<three_way_with_other, unsigned>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<unsigned, three_way_with_other>);
  ::testing::StaticAssertTypeEq<decltype(three_way_with_other{1} <= 2), bool>();
  STATIC_ASSERT_TRUE(noexcept(three_way_with_other{1} <= 2));
  ::testing::StaticAssertTypeEq<decltype(1 <= three_way_with_other{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(1 <= three_way_with_other{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// another type has `noexcept` _less than or equal_ operator returning expected results.
TEST(ThreeWayCompare, LessThanOrEqualOperatorReflectsResultOfThreeWayComparisonWithOther) {
  STATIC_ASSERT_LE(three_way_with_other{1}, 2);
  STATIC_ASSERT_FALSE(three_way_with_other{2} <= 1);
  STATIC_ASSERT_LE(three_way_with_other{1}, 1);
  STATIC_ASSERT_LE(1, three_way_with_other{2});
  STATIC_ASSERT_FALSE(2 <= three_way_with_other{1});
  STATIC_ASSERT_LE(1, three_way_with_other{1});
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// another type has `noexcept` _greater than or equal_ operator.
TEST(ThreeWayCompare, ATypeWithThreeWayCompareWithOtherHasGreaterthanOrEqualOperator) {
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<three_way_with_other>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<three_way_with_other, unsigned>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<unsigned, three_way_with_other>);
  ::testing::StaticAssertTypeEq<decltype(three_way_with_other{1} >= 2), bool>();
  STATIC_ASSERT_TRUE(noexcept(three_way_with_other{1} >= 2));
  ::testing::StaticAssertTypeEq<decltype(1 >= three_way_with_other{2}), bool>();
  STATIC_ASSERT_TRUE(noexcept(1 >= three_way_with_other{2}));
}

/// @test A type that inherits `full_ordering_operators_from_three_way_compare` and implements `three_way_compare` with
/// another type has `noexcept` _greater than or equal_ operator returning expected results.
TEST(ThreeWayCompare, GreaterthanOrEqualOperatorReflectsResultOfThreeWayComparisonWithOther) {
  STATIC_ASSERT_FALSE(three_way_with_other{1} >= 2);
  STATIC_ASSERT_GE(three_way_with_other{2}, 1);
  STATIC_ASSERT_GE(three_way_with_other{1}, 1);
  STATIC_ASSERT_FALSE(1 >= three_way_with_other{2});
  STATIC_ASSERT_GE(2, three_way_with_other{1});
  STATIC_ASSERT_GE(1, three_way_with_other{1});
}

namespace {
template <typename Tag>
struct int_holder {
  std::int32_t data;
};

struct tag1;
struct tag2;
struct tag3;
struct tag4;

class multi_three_way_comparable : arene::base::generic_ordering_from_three_way_compare<multi_three_way_comparable> {
  std::int32_t data_;

 public:
  constexpr explicit multi_three_way_comparable(std::int32_t data) noexcept
      : data_(data) {}

  static constexpr auto three_way_compare(
      multi_three_way_comparable const& lhs,
      multi_three_way_comparable const& rhs
  ) noexcept -> strong_ordering {
    return arene::base::compare_three_way{}(lhs.data_, rhs.data_);
  }
  static constexpr auto three_way_compare(multi_three_way_comparable const& lhs, int_holder<tag1> rhs) noexcept
      -> strong_ordering {
    return arene::base::compare_three_way{}(lhs.data_, rhs.data);
  }
  static constexpr auto three_way_compare(multi_three_way_comparable const& lhs, int_holder<tag2> rhs) noexcept
      -> strong_ordering {
    return arene::base::compare_three_way{}(lhs.data_, rhs.data);
  }
  static constexpr auto three_way_compare(multi_three_way_comparable const& lhs, int_holder<tag3> rhs) noexcept
      -> strong_ordering {
    return arene::base::compare_three_way{}(lhs.data_, rhs.data);
  }
};

}  // namespace

/// @test A type that inherits `generic_ordering_from_three_way_compare` and implements `three_way_compare` with its own
/// type and several other types has _equality_ operator for the supported comparisons returning expected results.
TEST(GeneratedOperators, TypeWithThreeWayCompareAndBaseClassHasAppropriateEqualityOperators) {
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<multi_three_way_comparable, int_holder<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<multi_three_way_comparable, int_holder<tag2>>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<multi_three_way_comparable, int_holder<tag3>>);
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<multi_three_way_comparable, int_holder<tag4>>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<int_holder<tag1>, multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<int_holder<tag2>, multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<int_holder<tag3>, multi_three_way_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<int_holder<tag4>, multi_three_way_comparable>);

  constexpr multi_three_way_comparable val{42};
  constexpr multi_three_way_comparable val2{43};
  STATIC_ASSERT_EQ(val, val);
  STATIC_ASSERT_FALSE(val == val2);
  STATIC_ASSERT_EQ(val, int_holder<tag1>{42});
  STATIC_ASSERT_EQ(int_holder<tag1>{42}, val);
  STATIC_ASSERT_FALSE(val == int_holder<tag1>{43});
  STATIC_ASSERT_FALSE(int_holder<tag1>{43} == val);

  STATIC_ASSERT_EQ(val, int_holder<tag2>{42});
  STATIC_ASSERT_EQ(int_holder<tag2>{42}, val);
  STATIC_ASSERT_FALSE(val == int_holder<tag2>{43});
  STATIC_ASSERT_FALSE(int_holder<tag2>{43} == val);

  STATIC_ASSERT_EQ(val, int_holder<tag3>{42});
  STATIC_ASSERT_EQ(int_holder<tag3>{42}, val);
  STATIC_ASSERT_FALSE(val == int_holder<tag3>{43});
  STATIC_ASSERT_FALSE(int_holder<tag3>{43} == val);
}

/// @test A type that inherits `generic_ordering_from_three_way_compare` and implements `three_way_compare` with its own
/// type and several other types has _inequality_ operator for the supported comparisons returning expected results.
TEST(GeneratedOperators, TypeWithThreeWayCompareAndBaseClassHasAppropriateInequalityOperators) {
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<multi_three_way_comparable, int_holder<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<multi_three_way_comparable, int_holder<tag2>>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<multi_three_way_comparable, int_holder<tag3>>);
  STATIC_ASSERT_FALSE(arene::base::is_inequality_comparable_v<multi_three_way_comparable, int_holder<tag4>>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<int_holder<tag1>, multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<int_holder<tag2>, multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<int_holder<tag3>, multi_three_way_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_inequality_comparable_v<int_holder<tag4>, multi_three_way_comparable>);

  constexpr multi_three_way_comparable val{43};
  constexpr multi_three_way_comparable val2{42};
  STATIC_ASSERT_NE(val, val2);
  STATIC_ASSERT_FALSE(val != val);

  STATIC_ASSERT_NE(val, int_holder<tag1>{42});
  STATIC_ASSERT_NE(int_holder<tag1>{42}, val);
  STATIC_ASSERT_FALSE(val != int_holder<tag1>{43});
  STATIC_ASSERT_FALSE(int_holder<tag1>{43} != val);

  STATIC_ASSERT_NE(val, int_holder<tag2>{42});
  STATIC_ASSERT_NE(int_holder<tag2>{42}, val);
  STATIC_ASSERT_FALSE(val != int_holder<tag2>{43});
  STATIC_ASSERT_FALSE(int_holder<tag2>{43} != val);

  STATIC_ASSERT_NE(val, int_holder<tag3>{42});
  STATIC_ASSERT_NE(int_holder<tag3>{42}, val);
  STATIC_ASSERT_FALSE(val != int_holder<tag3>{43});
  STATIC_ASSERT_FALSE(int_holder<tag3>{43} != val);
}

/// @test A type that inherits `generic_ordering_from_three_way_compare` and implements `three_way_compare` with its own
/// type and several other types has _less than_ operator for the supported comparisons returning expected results.
TEST(GeneratedOperators, TypeWithThreeWayCompareAndBaseClassHasAppropriateLessThanOperators) {
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<multi_three_way_comparable, int_holder<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<multi_three_way_comparable, int_holder<tag2>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<multi_three_way_comparable, int_holder<tag3>>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<multi_three_way_comparable, int_holder<tag4>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<int_holder<tag1>, multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<int_holder<tag2>, multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<int_holder<tag3>, multi_three_way_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<int_holder<tag4>, multi_three_way_comparable>);

  constexpr multi_three_way_comparable val{42};
  constexpr multi_three_way_comparable val2{43};
  STATIC_ASSERT_LT(val, val2);
  STATIC_ASSERT_FALSE(val < val);

  STATIC_ASSERT_LT(val, int_holder<tag1>{43});
  STATIC_ASSERT_LT(int_holder<tag1>{41}, val);
  STATIC_ASSERT_FALSE(val < int_holder<tag1>{41});
  STATIC_ASSERT_FALSE(int_holder<tag1>{43} < val);

  STATIC_ASSERT_LT(val, int_holder<tag2>{43});
  STATIC_ASSERT_LT(int_holder<tag2>{41}, val);
  STATIC_ASSERT_FALSE(val < int_holder<tag2>{41});
  STATIC_ASSERT_FALSE(int_holder<tag2>{43} < val);

  STATIC_ASSERT_LT(val, int_holder<tag3>{43});
  STATIC_ASSERT_LT(int_holder<tag3>{41}, val);
  STATIC_ASSERT_FALSE(val < int_holder<tag3>{41});
  STATIC_ASSERT_FALSE(int_holder<tag3>{43} < val);
}

/// @test A type that inherits `generic_ordering_from_three_way_compare` and implements `three_way_compare` with its own
/// type and several other types has _greater than_ operator for the supported comparisons returning expected results.
TEST(GeneratedOperators, TypeWithThreeWayCompareAndBaseClassHasAppropriateGreaterThanOperators) {
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<multi_three_way_comparable, int_holder<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<multi_three_way_comparable, int_holder<tag2>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<multi_three_way_comparable, int_holder<tag3>>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<multi_three_way_comparable, int_holder<tag4>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<int_holder<tag1>, multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<int_holder<tag2>, multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<int_holder<tag3>, multi_three_way_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<int_holder<tag4>, multi_three_way_comparable>);

  constexpr multi_three_way_comparable val{42};
  constexpr multi_three_way_comparable val2{43};
  STATIC_ASSERT_GT(val2, val);
  STATIC_ASSERT_FALSE(val > val);

  STATIC_ASSERT_GT(val, int_holder<tag1>{40});
  STATIC_ASSERT_GT(int_holder<tag1>{43}, val);
  STATIC_ASSERT_FALSE(val > int_holder<tag1>{43});
  STATIC_ASSERT_FALSE(int_holder<tag1>{40} > val);

  STATIC_ASSERT_GT(val, int_holder<tag2>{40});
  STATIC_ASSERT_GT(int_holder<tag2>{43}, val);
  STATIC_ASSERT_FALSE(val > int_holder<tag2>{43});
  STATIC_ASSERT_FALSE(int_holder<tag2>{40} > val);

  STATIC_ASSERT_GT(val, int_holder<tag3>{40});
  STATIC_ASSERT_GT(int_holder<tag3>{43}, val);
  STATIC_ASSERT_FALSE(val > int_holder<tag3>{43});
  STATIC_ASSERT_FALSE(int_holder<tag3>{40} > val);
}

/// @test A type that inherits `generic_ordering_from_three_way_compare` and implements `three_way_compare` with its own
/// type and several other types has _less than or equal_ operator for the supported comparisons returning expected
/// results.
TEST(GeneratedOperators, TypeWithThreeWayCompareAndBaseClassHasAppropriateLessThanOrEqualOperators) {
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<multi_three_way_comparable, int_holder<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<multi_three_way_comparable, int_holder<tag2>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<multi_three_way_comparable, int_holder<tag3>>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<multi_three_way_comparable, int_holder<tag4>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<int_holder<tag1>, multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<int_holder<tag2>, multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<int_holder<tag3>, multi_three_way_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<int_holder<tag4>, multi_three_way_comparable>);

  constexpr multi_three_way_comparable val{42};
  constexpr multi_three_way_comparable val2{43};
  STATIC_ASSERT_LE(val, val2);
  STATIC_ASSERT_LE(val, val);
  STATIC_ASSERT_FALSE(val2 <= val);

  STATIC_ASSERT_LE(val, int_holder<tag1>{43});
  STATIC_ASSERT_LE(val, int_holder<tag1>{42});
  STATIC_ASSERT_LE(int_holder<tag1>{41}, val);
  STATIC_ASSERT_LE(int_holder<tag1>{42}, val);
  STATIC_ASSERT_FALSE(val <= int_holder<tag1>{41});
  STATIC_ASSERT_FALSE(int_holder<tag1>{43} <= val);

  STATIC_ASSERT_LE(val, int_holder<tag2>{43});
  STATIC_ASSERT_LE(int_holder<tag2>{41}, val);
  STATIC_ASSERT_FALSE(val <= int_holder<tag2>{41});
  STATIC_ASSERT_FALSE(int_holder<tag2>{43} <= val);

  STATIC_ASSERT_LE(val, int_holder<tag3>{43});
  STATIC_ASSERT_LE(int_holder<tag3>{41}, val);
  STATIC_ASSERT_FALSE(val <= int_holder<tag3>{41});
  STATIC_ASSERT_FALSE(int_holder<tag3>{43} <= val);
}

/// @test A type that inherits `generic_ordering_from_three_way_compare` and implements `three_way_compare` with its own
/// type and several other types has _greater than or equal_ operator for the supported comparisons returning expected
/// results.
TEST(GeneratedOperators, TypeWithThreeWayCompareAndBaseClassHasAppropriateGreaterThanOrEqualOperators) {
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<multi_three_way_comparable, int_holder<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<multi_three_way_comparable, int_holder<tag2>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<multi_three_way_comparable, int_holder<tag3>>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<multi_three_way_comparable, int_holder<tag4>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<int_holder<tag1>, multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<int_holder<tag2>, multi_three_way_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<int_holder<tag3>, multi_three_way_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<int_holder<tag4>, multi_three_way_comparable>);

  constexpr multi_three_way_comparable val{43};
  constexpr multi_three_way_comparable val2{42};
  STATIC_ASSERT_GE(val, val2);
  STATIC_ASSERT_GE(val, val);
  STATIC_ASSERT_FALSE(val2 >= val);

  STATIC_ASSERT_GE(val, int_holder<tag1>{43});
  STATIC_ASSERT_GE(val, int_holder<tag1>{42});
  STATIC_ASSERT_GE(int_holder<tag1>{44}, val);
  STATIC_ASSERT_GE(int_holder<tag1>{43}, val);
  STATIC_ASSERT_FALSE(val >= int_holder<tag1>{44});
  STATIC_ASSERT_FALSE(int_holder<tag1>{42} >= val);

  STATIC_ASSERT_GE(val, int_holder<tag2>{42});
  STATIC_ASSERT_GE(int_holder<tag2>{44}, val);
  STATIC_ASSERT_FALSE(val >= int_holder<tag2>{44});
  STATIC_ASSERT_FALSE(int_holder<tag2>{42} >= val);

  STATIC_ASSERT_GE(val, int_holder<tag3>{42});
  STATIC_ASSERT_GE(int_holder<tag3>{44}, val);
  STATIC_ASSERT_FALSE(val >= int_holder<tag3>{44});
  STATIC_ASSERT_FALSE(int_holder<tag3>{42} >= val);
}

namespace {
class multi_three_way_comparable_with_equals
    : arene::base::generic_ordering_from_three_way_compare<multi_three_way_comparable_with_equals> {
  std::int32_t data_;

 public:
  constexpr explicit multi_three_way_comparable_with_equals(std::int32_t data) noexcept
      : data_(data) {}

  static constexpr auto fast_inequality_check(
      multi_three_way_comparable_with_equals const& lhs,
      multi_three_way_comparable_with_equals const& rhs
  ) noexcept -> arene::base::inequality_heuristic {
    return (lhs.data_ == rhs.data_) ? arene::base::inequality_heuristic::may_be_equal_or_not_equal
                                    : arene::base::inequality_heuristic::definitely_not_equal;
  }

  static constexpr auto fast_inequality_check(
      multi_three_way_comparable_with_equals const& lhs,
      int_holder<tag1> rhs
  ) noexcept -> arene::base::inequality_heuristic {
    return (lhs.data_ == rhs.data) ? arene::base::inequality_heuristic::may_be_equal_or_not_equal
                                   : arene::base::inequality_heuristic::definitely_not_equal;
  }

  static constexpr auto fast_inequality_check(
      multi_three_way_comparable_with_equals const& lhs,
      int_holder<tag3> rhs
  ) noexcept -> arene::base::inequality_heuristic {
    return (lhs.data_ == rhs.data) ? arene::base::inequality_heuristic::may_be_equal_or_not_equal
                                   : arene::base::inequality_heuristic::definitely_not_equal;
  }

  static constexpr auto three_way_compare(
      multi_three_way_comparable_with_equals const& lhs,
      multi_three_way_comparable_with_equals const& rhs
  ) noexcept -> strong_ordering {
    return arene::base::compare_three_way{}(lhs.data_, rhs.data_);
  }
  static constexpr auto three_way_compare(
      multi_three_way_comparable_with_equals const& lhs,
      int_holder<tag1> rhs
  ) noexcept -> strong_ordering {
    return arene::base::compare_three_way{}(lhs.data_, rhs.data);
  }
  static constexpr auto three_way_compare(
      multi_three_way_comparable_with_equals const& lhs,
      int_holder<tag3> rhs
  ) noexcept -> strong_ordering {
    return arene::base::compare_three_way{}(lhs.data_, rhs.data);
  }
};

}  // namespace

/// @test A type that inherits `generic_ordering_from_three_way_compare` and implements `three_way_compare` and
/// `fast_inequality_check` with its own type and several other types has expected operators for the supported
/// comparisons returning expected results.
TEST(GeneratedOperators, TypeWithThreeWayCompareAndEqualsAndBaseClassHasAppropriateOperators) {
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<
                     multi_three_way_comparable_with_equals,
                     int_holder<tag1>>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<
                      multi_three_way_comparable_with_equals,
                      int_holder<tag2>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<
                     multi_three_way_comparable_with_equals,
                     int_holder<tag3>>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<
                      multi_three_way_comparable_with_equals,
                      int_holder<tag4>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<
                     int_holder<tag1>,
                     multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<
                      int_holder<tag2>,
                      multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<
                     int_holder<tag3>,
                     multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<
                      int_holder<tag4>,
                      multi_three_way_comparable_with_equals>);

  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_less_than_or_equal_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag1>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_less_than_or_equal_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag2>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_less_than_or_equal_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag3>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_less_than_or_equal_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag4>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_less_than_or_equal_comparable_v<int_holder<tag1>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_less_than_or_equal_comparable_v<int_holder<tag2>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_less_than_or_equal_comparable_v<int_holder<tag3>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_less_than_or_equal_comparable_v<int_holder<tag4>, multi_three_way_comparable_with_equals>);

  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_greater_than_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag1>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_greater_than_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag2>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_greater_than_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag3>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_greater_than_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag4>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_greater_than_comparable_v<int_holder<tag1>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_greater_than_comparable_v<int_holder<tag2>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_greater_than_comparable_v<int_holder<tag3>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_greater_than_comparable_v<int_holder<tag4>, multi_three_way_comparable_with_equals>);

  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag1>>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag2>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag3>>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag4>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<int_holder<tag1>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<int_holder<tag2>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<int_holder<tag3>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<int_holder<tag4>, multi_three_way_comparable_with_equals>);

  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag1>>);
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag2>>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag3>>);
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag4>>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<int_holder<tag1>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<int_holder<tag2>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<int_holder<tag3>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<int_holder<tag4>, multi_three_way_comparable_with_equals>);

  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag1>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_inequality_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag2>>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag3>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_inequality_comparable_v<multi_three_way_comparable_with_equals, int_holder<tag4>>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<int_holder<tag1>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_inequality_comparable_v<int_holder<tag2>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<int_holder<tag3>, multi_three_way_comparable_with_equals>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_inequality_comparable_v<int_holder<tag4>, multi_three_way_comparable_with_equals>);

  STATIC_ASSERT_EQ(multi_three_way_comparable_with_equals{42}, multi_three_way_comparable_with_equals{42});
  STATIC_ASSERT_NE(multi_three_way_comparable_with_equals{42}, multi_three_way_comparable_with_equals{43});
  STATIC_ASSERT_LT(multi_three_way_comparable_with_equals{42}, multi_three_way_comparable_with_equals{43});
  STATIC_ASSERT_LE(multi_three_way_comparable_with_equals{42}, multi_three_way_comparable_with_equals{42});
  STATIC_ASSERT_GT(multi_three_way_comparable_with_equals{42}, multi_three_way_comparable_with_equals{41});
  STATIC_ASSERT_GE(multi_three_way_comparable_with_equals{42}, multi_three_way_comparable_with_equals{42});

  STATIC_ASSERT_EQ(multi_three_way_comparable_with_equals{42}, int_holder<tag1>{42});
  STATIC_ASSERT_NE(multi_three_way_comparable_with_equals{42}, int_holder<tag1>{43});
  STATIC_ASSERT_LT(multi_three_way_comparable_with_equals{42}, int_holder<tag1>{43});
  STATIC_ASSERT_LE(multi_three_way_comparable_with_equals{42}, int_holder<tag1>{42});
  STATIC_ASSERT_GT(multi_three_way_comparable_with_equals{42}, int_holder<tag1>{41});
  STATIC_ASSERT_GE(multi_three_way_comparable_with_equals{42}, int_holder<tag1>{42});

  STATIC_ASSERT_EQ(int_holder<tag1>{42}, multi_three_way_comparable_with_equals{42});
  STATIC_ASSERT_NE(int_holder<tag1>{42}, multi_three_way_comparable_with_equals{43});
  STATIC_ASSERT_LT(int_holder<tag1>{42}, multi_three_way_comparable_with_equals{43});
  STATIC_ASSERT_LE(int_holder<tag1>{42}, multi_three_way_comparable_with_equals{42});
  STATIC_ASSERT_GT(int_holder<tag1>{42}, multi_three_way_comparable_with_equals{41});
  STATIC_ASSERT_GE(int_holder<tag1>{42}, multi_three_way_comparable_with_equals{42});

  STATIC_ASSERT_EQ(multi_three_way_comparable_with_equals{42}, int_holder<tag3>{42});
  STATIC_ASSERT_NE(multi_three_way_comparable_with_equals{42}, int_holder<tag3>{43});
  STATIC_ASSERT_LT(multi_three_way_comparable_with_equals{42}, int_holder<tag3>{43});
  STATIC_ASSERT_LE(multi_three_way_comparable_with_equals{42}, int_holder<tag3>{42});
  STATIC_ASSERT_GT(multi_three_way_comparable_with_equals{42}, int_holder<tag3>{41});
  STATIC_ASSERT_GE(multi_three_way_comparable_with_equals{42}, int_holder<tag3>{42});

  STATIC_ASSERT_EQ(int_holder<tag3>{42}, multi_three_way_comparable_with_equals{42});
  STATIC_ASSERT_NE(int_holder<tag3>{42}, multi_three_way_comparable_with_equals{43});
  STATIC_ASSERT_LT(int_holder<tag3>{42}, multi_three_way_comparable_with_equals{43});
  STATIC_ASSERT_LE(int_holder<tag3>{42}, multi_three_way_comparable_with_equals{42});
  STATIC_ASSERT_GT(int_holder<tag3>{42}, multi_three_way_comparable_with_equals{41});
  STATIC_ASSERT_GE(int_holder<tag3>{42}, multi_three_way_comparable_with_equals{42});
}

namespace {
template <typename T>
class template_comparable : arene::base::generic_ordering_from_three_way_compare<template_comparable<T>> {
  std::int32_t data_;

 public:
  explicit template_comparable(std::int32_t data) noexcept
      : data_(data) {}

  template <typename U, arene::base::constraints<std::enable_if_t<!std::is_same<T, U>::value>> = nullptr>
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  template_comparable(template_comparable<U> const& other) noexcept
      : data_(other.get_data()) {}

  auto get_data() const noexcept -> std::int32_t { return data_; }

  static auto fast_inequality_check(template_comparable const& lhs, template_comparable const& rhs) noexcept
      -> arene::base::inequality_heuristic {
    return (lhs.data_ == rhs.data_) ? arene::base::inequality_heuristic::may_be_equal_or_not_equal
                                    : arene::base::inequality_heuristic::definitely_not_equal;
  }

  template <typename U, arene::base::constraints<std::enable_if_t<!std::is_same<T, U>::value>> = nullptr>
  static auto fast_inequality_check(template_comparable const& lhs, template_comparable<U> const& rhs) noexcept
      -> arene::base::inequality_heuristic {
    return (lhs.data_ == rhs.get_data()) ? arene::base::inequality_heuristic::may_be_equal_or_not_equal
                                         : arene::base::inequality_heuristic::definitely_not_equal;
    ;
  }

  static auto three_way_compare(template_comparable const& lhs, template_comparable const& rhs) noexcept
      -> strong_ordering {
    return arene::base::compare_three_way{}(lhs.data_, rhs.data_);
  }

  template <typename U, arene::base::constraints<std::enable_if_t<!std::is_same<T, U>::value>> = nullptr>
  static auto three_way_compare(template_comparable const& lhs, template_comparable<U> const& rhs) noexcept
      -> strong_ordering {
    return arene::base::compare_three_way{}(lhs.data_, rhs.get_data());
  }

  template <typename U>
  static auto fast_inequality_check(template_comparable const& lhs, int_holder<U> const& rhs) noexcept
      -> arene::base::inequality_heuristic {
    return (lhs.data_ == rhs.data) ? arene::base::inequality_heuristic::may_be_equal_or_not_equal
                                   : arene::base::inequality_heuristic::definitely_not_equal;
    ;
  }

  template <typename U>
  static auto three_way_compare(template_comparable const& lhs, int_holder<U> const& rhs) noexcept -> strong_ordering {
    return arene::base::compare_three_way{}(lhs.data_, rhs.data);
  }
};
}  // namespace

/// @test A type that inherits `generic_ordering_from_three_way_compare` and implements `three_way_compare` and
/// `fast_inequality_check` with its own type and several other types, including types derived from
/// `generic_ordering_from_three_way_compare`, has expected operators for the supported comparisons returning expected
/// results.
TEST(GeneratedOperators, TemplateTypeWithThreeWayCompareAndEqualsAndBaseClassHasAppropriateOperators) {
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<template_comparable<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_greater_than_or_equal_comparable_v<template_comparable<tag1>, template_comparable<tag2>>);

  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<template_comparable<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_less_than_or_equal_comparable_v<template_comparable<tag1>, template_comparable<tag2>>);

  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<template_comparable<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<template_comparable<tag1>, template_comparable<tag2>>);

  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<template_comparable<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<template_comparable<tag1>, template_comparable<tag2>>);

  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<template_comparable<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<template_comparable<tag1>, template_comparable<tag2>>);

  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<template_comparable<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<template_comparable<tag1>, template_comparable<tag2>>);

  ASSERT_EQ(template_comparable<tag1>{42}, template_comparable<tag2>{42});
  ASSERT_NE(template_comparable<tag1>{42}, template_comparable<tag2>{43});

  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<int_holder<tag2>, template_comparable<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<template_comparable<tag1>, int_holder<tag2>>);

  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<int_holder<tag2>, template_comparable<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<template_comparable<tag1>, int_holder<tag2>>);

  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<int_holder<tag2>, template_comparable<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<template_comparable<tag1>, int_holder<tag2>>);

  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<int_holder<tag2>, template_comparable<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<template_comparable<tag1>, int_holder<tag2>>);

  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<int_holder<tag2>, template_comparable<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<template_comparable<tag1>, int_holder<tag2>>);

  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<int_holder<tag2>, template_comparable<tag1>>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<template_comparable<tag1>, int_holder<tag2>>);

  ASSERT_EQ(template_comparable<tag1>{42}, int_holder<tag2>{42});
  ASSERT_NE(int_holder<tag1>{42}, template_comparable<tag2>{43});
  ASSERT_LT(template_comparable<tag1>{42}, int_holder<tag2>{43});
  ASSERT_GT(int_holder<tag1>{43}, template_comparable<tag2>{42});
}

/// @test Ensure that `fast_inequality_check` is used when defined in a child class of
/// `generic_ordering_from_three_way_compare` for expected operations only.
TEST(GeneratedOperators, EnsureFastInequalityCheckIsUsed) {
  struct three_way_with_odd_equality
      : arene::base::generic_ordering_from_three_way_compare<three_way_with_odd_equality> {
    unsigned value;

    constexpr explicit three_way_with_odd_equality(unsigned that_value) noexcept
        : value(that_value) {}

    static auto three_way_compare(three_way_with_odd_equality lhs, three_way_with_odd_equality rhs) noexcept
        -> strong_ordering {
      return (lhs.value < rhs.value) ? strong_ordering::less
                                     : ((lhs.value == rhs.value) ? strong_ordering::equal : strong_ordering::greater);
    }

    /// Counterintuitive inverse equality to ensure it is used
    static auto fast_inequality_check(three_way_with_odd_equality lhs, three_way_with_odd_equality rhs) noexcept
        -> arene::base::inequality_heuristic {
      return lhs.value == rhs.value ? arene::base::inequality_heuristic::definitely_not_equal
                                    : arene::base::inequality_heuristic::may_be_equal_or_not_equal;
    }
  };

  STATIC_ASSERT_TRUE(arene::base::has_fast_inequality_check_v<three_way_with_odd_equality>);

  ASSERT_NE(three_way_with_odd_equality{42}, three_way_with_odd_equality{42});
  ASSERT_FALSE(three_way_with_odd_equality{42} == three_way_with_odd_equality{42});
  ASSERT_NE(three_way_with_odd_equality{42}, three_way_with_odd_equality{43});
  ASSERT_FALSE(three_way_with_odd_equality{42} == three_way_with_odd_equality{43});
  ASSERT_LT(three_way_with_odd_equality{42}, three_way_with_odd_equality{43});
  ASSERT_LE(three_way_with_odd_equality{42}, three_way_with_odd_equality{42});
  ASSERT_GE(three_way_with_odd_equality{42}, three_way_with_odd_equality{42});
  ASSERT_FALSE(three_way_with_odd_equality{42} < three_way_with_odd_equality{42});
  ASSERT_FALSE(three_way_with_odd_equality{42} > three_way_with_odd_equality{42});
}

/// @test Ensure that `fast_inequality_check` with another type is used when defined in a child class of
/// `generic_ordering_from_three_way_compare`.
TEST(GeneratedOperators, EnsureFastInequalityCheckUsedAsNeeded) {
  struct three_way_with_odd_equality
      : arene::base::generic_ordering_from_three_way_compare<three_way_with_odd_equality> {
    std::int32_t value;

    constexpr explicit three_way_with_odd_equality(std::int32_t that_value) noexcept
        : value(that_value) {}

    static auto three_way_compare(three_way_with_odd_equality lhs, int_holder<tag1> rhs) noexcept -> strong_ordering {
      return (lhs.value < rhs.data) ? strong_ordering::less
                                    : ((lhs.value == rhs.data) ? strong_ordering::equal : strong_ordering::greater);
    }

    static auto three_way_compare(three_way_with_odd_equality lhs, int_holder<tag2> rhs) noexcept -> strong_ordering {
      return (lhs.value < rhs.data) ? strong_ordering::less
                                    : ((lhs.value == rhs.data) ? strong_ordering::equal : strong_ordering::greater);
    }

    static auto fast_inequality_check(three_way_with_odd_equality lhs, int_holder<tag1>) noexcept
        -> arene::base::inequality_heuristic {
      return (lhs.value % 16) == 3 ? arene::base::inequality_heuristic::definitely_not_equal
                                   : arene::base::inequality_heuristic::may_be_equal_or_not_equal;
    }
  };

  ASSERT_NE(three_way_with_odd_equality{0x3}, int_holder<tag1>{0x3});
  ASSERT_NE(three_way_with_odd_equality{0x13}, int_holder<tag1>{0x13});
  ASSERT_EQ(three_way_with_odd_equality{0x14}, int_holder<tag1>{0x14});
  ASSERT_NE(three_way_with_odd_equality{0x33}, int_holder<tag1>{0x33});
  ASSERT_EQ(three_way_with_odd_equality{0x34}, int_holder<tag1>{0x34});
  ASSERT_EQ(three_way_with_odd_equality{0x33}, int_holder<tag2>{0x33});
  ASSERT_EQ(three_way_with_odd_equality{0x34}, int_holder<tag2>{0x34});
}

template <typename T>
struct convertible_from_t : arene::base::generic_ordering_from_three_way_compare<convertible_from_t<T>> {
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  convertible_from_t(T const&) {}

  static auto three_way_compare(convertible_from_t, convertible_from_t) noexcept -> strong_ordering {
    return strong_ordering::equal;
  }

  template <
      typename U,
      arene::base::constraints<std::enable_if_t<arene::base::compare_three_way_supported_v<T, U>>> = nullptr>
  static auto three_way_compare(convertible_from_t, U const&) noexcept -> strong_ordering {
    return strong_ordering::equal;
  }
};

template <typename T>
struct other_template {};

/// @test Types that inherit `generic_ordering_from_three_way_compare` and have a _converting constructor_ support
/// three-way comparison and comparison operations correctly. Operators are not generated for types that have the former
/// type as a template parameter.
TEST(GeneratedOperators, ImplicitlyConvertibleTypesDoNotTriggerGenericOperators) {
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<convertible_from_t<int>, convertible_from_t<int>>);
  STATIC_ASSERT_TRUE(arene::base::compare_three_way_supported_v<convertible_from_t<int>, double>);
  STATIC_ASSERT_FALSE(arene::base::compare_three_way_supported_v<int, other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_TRUE(arene::base::is_three_way_comparable_v<convertible_from_t<int>, convertible_from_t<int>>);
  STATIC_ASSERT_TRUE(arene::base::is_three_way_comparable_v<convertible_from_t<int>, double>);
  STATIC_ASSERT_FALSE(arene::base::is_three_way_comparable_v<int, other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<int, other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<int, other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<int, other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<int, other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<int, other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::is_inequality_comparable_v<int, other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<other_template<convertible_from_t<int>>, int>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<other_template<convertible_from_t<int>>, int>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<other_template<convertible_from_t<int>>, int>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<other_template<convertible_from_t<int>>, int>);
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<other_template<convertible_from_t<int>>, int>);
  STATIC_ASSERT_FALSE(arene::base::is_inequality_comparable_v<other_template<convertible_from_t<int>>, int>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<int, convertible_from_t<int>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<int, convertible_from_t<int>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<int, convertible_from_t<int>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<int, convertible_from_t<int>>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<int, convertible_from_t<int>>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<int, convertible_from_t<int>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<convertible_from_t<int>, int>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<convertible_from_t<int>, int>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<convertible_from_t<int>, int>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<convertible_from_t<int>, int>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<convertible_from_t<int>, int>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<convertible_from_t<int>, int>);

  STATIC_ASSERT_FALSE(arene::base::compare_three_way_supported_v<
                      convertible_from_t<int>,
                      other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_three_way_comparable_v<convertible_from_t<int>, other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_less_than_comparable_v<convertible_from_t<int>, other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<
                      convertible_from_t<int>,
                      other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<
                      convertible_from_t<int>,
                      other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<
                      convertible_from_t<int>,
                      other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_equality_comparable_v<convertible_from_t<int>, other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_inequality_comparable_v<convertible_from_t<int>, other_template<convertible_from_t<int>>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_less_than_comparable_v<other_template<convertible_from_t<int>>, convertible_from_t<int>>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<
                      other_template<convertible_from_t<int>>,
                      convertible_from_t<int>>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<
                      other_template<convertible_from_t<int>>,
                      convertible_from_t<int>>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<
                      other_template<convertible_from_t<int>>,
                      convertible_from_t<int>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_equality_comparable_v<other_template<convertible_from_t<int>>, convertible_from_t<int>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_inequality_comparable_v<other_template<convertible_from_t<int>>, convertible_from_t<int>>);

  STATIC_ASSERT_TRUE(arene::base::is_invocable_v<arene::base::compare_three_way, int, convertible_from_t<int>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_invocable_v<arene::base::compare_three_way, int, other_template<convertible_from_t<int>>>);
}

/// A test class that uses the mixin which uses @c operator== for equality and inequality, but uses @c three_way_compare
/// for ordered comparisons.
class has_three_way_compare_and_equals
    : arene::base::generic_ordering_from_three_way_compare_and_equals<has_three_way_compare_and_equals> {
  std::int32_t data_;
  mutable bool did_equals_{false};
  mutable bool did_three_way_compare_{false};

 public:
  constexpr explicit has_three_way_compare_and_equals(std::int32_t data) noexcept
      : data_(data) {}

  friend constexpr auto operator==(
      has_three_way_compare_and_equals const& left,
      has_three_way_compare_and_equals const& right
  ) noexcept -> bool {
    left.did_equals_ = true;
    right.did_equals_ = true;
    return left.data_ == right.data_;
  }

  template <
      typename Other,
      arene::base::constraints<
          std::enable_if_t<!std::is_same<Other, has_three_way_compare_and_equals>::value>,
          std::enable_if_t<arene::base::is_equality_comparable_v<std::int32_t, Other const&>>> = nullptr>
  friend constexpr auto operator==(has_three_way_compare_and_equals const& self, Other const& other) noexcept -> bool {
    self.did_equals_ = true;
    return self.data_ == other;
  }

  static constexpr auto three_way_compare(
      has_three_way_compare_and_equals const& lhs,
      has_three_way_compare_and_equals const& rhs
  ) noexcept -> strong_ordering {
    lhs.did_three_way_compare_ = true;
    rhs.did_three_way_compare_ = true;
    return arene::base::compare_three_way{}(lhs.data_, rhs.data_);
  }

  template <
      typename Other,
      arene::base::constraints<
          std::enable_if_t<!std::is_same<Other, has_three_way_compare_and_equals>::value>,
          std::enable_if_t<arene::base::is_less_than_comparable_v<std::int32_t, Other const&>>> = nullptr>
  static constexpr auto three_way_compare(has_three_way_compare_and_equals const& lhs, Other const& rhs) noexcept
      -> strong_ordering {
    lhs.did_three_way_compare_ = true;
    return arene::base::compare_three_way{}(lhs.data_, rhs);
  }

  /// We want to test that this is never used.
  template <typename U>
  ARENE_NORETURN static auto fast_inequality_check(has_three_way_compare_and_equals const&, U const&) noexcept
      -> arene::base::inequality_heuristic {
    ARENE_INVARIANT_UNREACHABLE("Called fast_inequality_check instead of operator==");
  }

  /// @brief Check if this instance was involved in an @c operator== check, then reset its tracker
  /// @return @c true if this instance was involved in an @c operator== invocation, @c false if not
  constexpr auto did_equals() const noexcept -> bool {
    bool const temp = did_equals_;
    did_equals_ = false;
    return temp;
  }

  /// @brief Check if this instance was involved in a @c three_way_compare check, then reset its tracker
  /// @return @c true if this instance was involved in a @c three_way_compare invocation, @c false if not
  constexpr auto did_three_way_compare() const noexcept -> bool {
    bool const temp = did_three_way_compare_;
    did_three_way_compare_ = false;
    return temp;
  }
};

/// @test A type that inherits `generic_ordering_from_three_way_compare_and_equals` and implements `three_way_compare`
/// and `operator==` has expected operators from the supported comparisons returning expected results.
TEST(GeneratedOperators, TypeWithEqualsFallingBackToThreeWayCompareHasAppropriateOperators) {
  using test_type = has_three_way_compare_and_equals;

  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<test_type>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<test_type, int>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<int, test_type>);

  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<test_type>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<test_type, int>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<int, test_type>);

  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<test_type>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<test_type, int>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<int, test_type>);

  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<test_type>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<test_type, int>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<int, test_type>);

  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<test_type>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<test_type, int>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<int, test_type>);

  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<test_type>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<test_type, int>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<int, test_type>);
}

/// @test A type that inherits `generic_ordering_from_three_way_compare_and_equals` and implements `three_way_compare`
/// and `operator==` invokes the expected operators when doing self-comparisons.
TEST(GeneratedOperators, TypeWithEqualsFallingBackToThreeWayCompareCallsCorrectOperators) {
  using test_type = has_three_way_compare_and_equals;

  test_type const one{1};
  test_type const one_again{1};
  ASSERT_EQ(one, one_again);
  ASSERT_TRUE(one.did_equals());
  ASSERT_TRUE(one_again.did_equals());
  ASSERT_FALSE(one.did_three_way_compare());
  ASSERT_FALSE(one_again.did_three_way_compare());

  test_type const two{2};
  ASSERT_NE(one, two);
  ASSERT_TRUE(one.did_equals());
  ASSERT_TRUE(two.did_equals());
  ASSERT_FALSE(one.did_three_way_compare());
  ASSERT_FALSE(two.did_three_way_compare());

  ASSERT_LT(one, two);
  ASSERT_FALSE(one.did_equals());
  ASSERT_FALSE(two.did_equals());
  ASSERT_TRUE(one.did_three_way_compare());
  ASSERT_TRUE(two.did_three_way_compare());

  ASSERT_LE(one, one_again);
  ASSERT_FALSE(one.did_equals());
  ASSERT_FALSE(one_again.did_equals());
  ASSERT_TRUE(one.did_three_way_compare());
  ASSERT_TRUE(one_again.did_three_way_compare());

  ASSERT_GT(two, one);
  ASSERT_FALSE(one.did_equals());
  ASSERT_FALSE(two.did_equals());
  ASSERT_TRUE(one.did_three_way_compare());
  ASSERT_TRUE(two.did_three_way_compare());

  ASSERT_GE(one, one_again);
  ASSERT_FALSE(one.did_equals());
  ASSERT_FALSE(one_again.did_equals());
  ASSERT_TRUE(one.did_three_way_compare());
  ASSERT_TRUE(one_again.did_three_way_compare());
}

/// @test A type that inherits `generic_ordering_from_three_way_compare_and_equals` and implements `three_way_compare`
/// and `operator==` invokes the expected operators when doing non-self-comparisons.
TEST(GeneratedOperators, TypeWithEqualsFallingBackToThreeWayCompareCallsCorrectOperatorsForTemplateComparison) {
  has_three_way_compare_and_equals const tester{1};
  int const one{1};
  ASSERT_EQ(tester, one);
  ASSERT_TRUE(tester.did_equals());
  ASSERT_FALSE(tester.did_three_way_compare());
  ASSERT_EQ(one, tester);
  ASSERT_TRUE(tester.did_equals());
  ASSERT_FALSE(tester.did_three_way_compare());

  int const two{2};
  ASSERT_NE(tester, two);
  ASSERT_TRUE(tester.did_equals());
  ASSERT_FALSE(tester.did_three_way_compare());
  ASSERT_NE(two, tester);
  ASSERT_TRUE(tester.did_equals());
  ASSERT_FALSE(tester.did_three_way_compare());

  ASSERT_LT(tester, two);
  ASSERT_FALSE(tester.did_equals());
  ASSERT_TRUE(tester.did_three_way_compare());

  ASSERT_LE(tester, two);
  ASSERT_FALSE(tester.did_equals());
  ASSERT_TRUE(tester.did_three_way_compare());
  ASSERT_LE(one, tester);
  ASSERT_FALSE(tester.did_equals());
  ASSERT_TRUE(tester.did_three_way_compare());

  ASSERT_GT(two, tester);
  ASSERT_FALSE(tester.did_equals());
  ASSERT_TRUE(tester.did_three_way_compare());

  ASSERT_GE(two, tester);
  ASSERT_FALSE(tester.did_equals());
  ASSERT_TRUE(tester.did_three_way_compare());
  ASSERT_GE(tester, one);
  ASSERT_FALSE(tester.did_equals());
  ASSERT_TRUE(tester.did_three_way_compare());
}

/// @brief A type that's implicitly convertible to @c int but does not have its own comparisons defined
class convertible_to_int {
 private:
  int value_;

 public:
  explicit constexpr convertible_to_int(int value) noexcept
      : value_(value) {}

  // NOLINTNEXTLINE(hicpp-explicit-conversions) We're specifically testing implicit conversions here
  constexpr operator int() const noexcept { return value_; }
};

/// @test A type that inherits `generic_ordering_from_three_way_compare_and_equals` and implements `three_way_compare`
/// and `operator==` is fully comparable with a type that's implicitly convertible to one with comparisons defined.
TEST(GeneratedOperators, TypeWithEqualsFallingBackToThreeWayCompareCanUseImplicitConversion) {
  using test_type = has_three_way_compare_and_equals;

  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<test_type, convertible_to_int>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<convertible_to_int, test_type>);

  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<test_type, convertible_to_int>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<convertible_to_int, test_type>);

  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<test_type, convertible_to_int>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<convertible_to_int, test_type>);

  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<test_type, convertible_to_int>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<convertible_to_int, test_type>);

  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<test_type, convertible_to_int>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<convertible_to_int, test_type>);

  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<test_type, convertible_to_int>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<convertible_to_int, test_type>);

  ASSERT_EQ(test_type{0}, convertible_to_int{0});
  ASSERT_NE(convertible_to_int{-1}, test_type{1});
  ASSERT_LT(test_type{-1}, convertible_to_int{1});
  ASSERT_LE(convertible_to_int{0}, test_type{0});
  ASSERT_GT(test_type{1}, convertible_to_int{-1});
  ASSERT_GE(convertible_to_int{0}, test_type{0});
}

/// @brief A class with SFINAE-friendly templated equality so that its handwritten equality operator doesn't have higher
/// overload priority than the one(s) from the mixin
class has_sfinae_friendly_equality
    : arene::base::generic_ordering_from_three_way_compare_and_equals<has_sfinae_friendly_equality> {
  int value_;

 public:
  constexpr explicit has_sfinae_friendly_equality(int value) noexcept
      : value_(value) {}

  template <
      typename Self,
      typename Other,
      arene::base::constraints<
          std::enable_if_t<std::is_same<Self, has_sfinae_friendly_equality>::value>,
          std::enable_if_t<arene::base::is_equality_comparable_v<int, Other>>> = nullptr>
  friend constexpr auto operator==(Self const& self, Other const& other) noexcept -> bool {
    return self.value_ == other;
  }
};

/// @test A type inheriting `generic_ordering_from_three_way_compare_and_equals` which implements its equality operator
/// as a hidden friend with both parameters templated (to enable SFINAE) still has the expected operations working.
TEST(GeneratedOperators, TypeWithSfinaeFriendlyEqualityWorksWithEqualityMixin) {
  using test_type = has_sfinae_friendly_equality;

  // This call would be ambiguous without the "eligible_for_reversed_equals_with" trait.
  STATIC_ASSERT_EQ(test_type{5}, test_type{5});
  STATIC_ASSERT_NE(test_type{5}, test_type{9});

  STATIC_ASSERT_EQ(test_type{5}, 5);
  STATIC_ASSERT_NE(test_type{5}, 9);

  STATIC_ASSERT_EQ(5, test_type{5});
  STATIC_ASSERT_NE(9, test_type{5});
}

}  // namespace
