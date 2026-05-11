// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/type_traits/comparison_traits.hpp"

#include <string>

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"

namespace {

ARENE_IGNORE_START();
ARENE_IGNORE_CLANG(
    "-Wunneeded-internal-declaration",
    "Trait detection on declaration is possible, but triggers this warning. You cannot ARENE_MAYBE_UNUSED "
    "a declaration. And we cannot provide a definition without getting uncovered lines as a definition "
    "would never be invoked. Therefore we have to suppress this warning via diagnostics."
);

struct non_comparable {};

struct less_than_comparable {
  auto operator<(less_than_comparable const&) noexcept -> bool;
  auto operator<(int) const noexcept -> bool;

  friend auto operator<(std::string const&, less_than_comparable const&) noexcept -> bool;
};

struct greater_than_comparable {
  friend auto operator>(greater_than_comparable const&, greater_than_comparable const&) noexcept -> bool;
  friend auto operator>(greater_than_comparable const&, int) noexcept -> bool;
};

struct gte {
  friend auto operator>=(gte const&, gte const&) noexcept -> bool;
  friend auto operator>=(gte const&, std::string const&) noexcept -> bool;
};

struct lte {
  friend auto operator<=(lte const&, lte const&) noexcept -> bool;
  friend auto operator<=(std::string const&, lte) noexcept -> bool;
};

struct eq {
  friend auto operator==(eq const&, eq const&) noexcept -> bool;
  friend auto operator==(eq const&, lte const&) noexcept -> bool;
};

struct ne {
  friend auto operator!=(ne const&, ne const&) noexcept -> bool;
  friend auto operator!=(ne const&, double) noexcept -> bool;
};

struct throwing_less_than_comparable {
  auto operator<(throwing_less_than_comparable const&) -> bool;
  auto operator<(int) const -> bool;

  friend auto operator<(std::string const&, throwing_less_than_comparable const&) -> bool;
};

struct throwing_greater_than_comparable {
  friend auto operator>(throwing_greater_than_comparable const&, throwing_greater_than_comparable const&) -> bool;
  friend auto operator>(throwing_greater_than_comparable const&, int) -> bool;
};

struct throwing_gte {
  friend auto operator>=(throwing_gte const&, throwing_gte const&) -> bool;
  friend auto operator>=(throwing_gte const&, std::string const&) -> bool;
};

struct throwing_lte {
  friend auto operator<=(throwing_lte const&, throwing_lte const&) -> bool;
  friend auto operator<=(std::string const&, throwing_lte) -> bool;
};

struct throwing_eq {
  friend auto operator==(throwing_eq const&, throwing_eq const&) -> bool;
  friend auto operator==(throwing_eq const&, throwing_lte const&) -> bool;
};

struct throwing_ne {
  friend auto operator!=(throwing_ne const&, throwing_ne const&) -> bool;
  friend auto operator!=(throwing_ne const&, double) -> bool;
};

ARENE_IGNORE_END();

/// @test `is_less_than_comparable_v` evaluates to `false` when the parameter types cannot be compared using the
/// less-than operator
TEST(IsLessThanComparable, FalseForTypesWithoutLessThanOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<non_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<gte>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<less_than_comparable const>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<int, less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<less_than_comparable, std::string>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<throwing_greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<throwing_gte>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<throwing_less_than_comparable const>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<int, throwing_less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<throwing_less_than_comparable, std::string>);
}

/// @test `is_less_than_comparable_v` evaluates to `true` when the parameter types can be compared using the less-than
/// operator
TEST(IsLessThanComparable, TrueForTypesWithLessThanOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<std::string>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<less_than_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<less_than_comparable const, int>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<std::string, less_than_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<throwing_less_than_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<throwing_less_than_comparable const, int>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<std::string, throwing_less_than_comparable>);
}

/// @test `is_less_than_or_equal_comparable_v` evaluates to `false` when the parameter types cannot be compared using
/// the less-than-or-equal operator
TEST(IsLessThanOrEqualComparable, FalseForTypesWithoutLessThanOrEqualOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<non_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<int, lte>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<lte, std::string>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<throwing_less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<throwing_greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<int, throwing_lte>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<throwing_lte, std::string>);
}

/// @test `is_less_than_or_equal_comparable_v` evaluates to `true` when the parameter types can be compared using
/// the less-than-or-equal operator
TEST(IsLessThanOrEqualComparable, TrueForTypesWithLessThanOrEqualOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<lte>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<std::string, lte>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<throwing_lte>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<std::string, throwing_lte>);
}

/// @test `is_greater_than_comparable_v` evaluates to `false` when the parameter types cannot be compared using
/// the greater-than operator
TEST(IsGreaterThanComparable, FalseForTypesWithoutGreaterThanOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<non_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<int, greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<gte>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<int, throwing_greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<throwing_less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<throwing_gte>);
}

/// @test `is_greater_than_comparable_v` evaluates to `true` when the parameter types can be compared using
/// the greater-than operator
TEST(IsGreaterThanComparable, TrueForTypesWithGreaterThanOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<greater_than_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<greater_than_comparable, int>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<throwing_greater_than_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<throwing_greater_than_comparable, int>);
}

/// @test `is_greater_than_or_equal_comparable_v` evaluates to `false` when the parameter types cannot be compared using
/// the greater-than-or-equal operator
TEST(IsGreaterThanOrEqualComparable, FalseForTypesWithoutGreaterThanOrEqualOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<non_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<std::string, gte>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<gte, int>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<std::string, throwing_gte>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<throwing_gte, int>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<throwing_greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<throwing_less_than_comparable>);
}

/// @test `is_greater_than_or_equal_comparable_v` evaluates to `true` when the parameter types can be compared using
/// the greater-than-or-equal operator
TEST(IsGreaterThanOrEqualComparable, TrueForTypesWithGreaterThanOrEqualOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<gte>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<gte, std::string>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<throwing_gte>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<throwing_gte, std::string>);
}

/// @test `is_equality_comparable_v` evaluates to `false` when the parameter types cannot be compared using the equality
/// operator
TEST(IsEqualityComparable, FalseForTypesWithoutEqualityOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<non_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<lte>);
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<lte, eq>);
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<throwing_lte>);
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<throwing_lte, throwing_eq>);
}

/// @test `is_equality_comparable_v` evaluates to `true` when the parameter types can be compared using the equality
/// operator
TEST(IsEqualityComparable, TrueForTypesWithEqualityOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<std::string>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<eq>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<eq, lte>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<throwing_eq>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<throwing_eq, throwing_lte>);
}

/// @test `is_inequality_comparable_v` evaluates to `false` when the parameter types cannot be compared using the
/// inequality operator
TEST(IsInEqualityComparable, FalseForTypesWithoutInequalityOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_inequality_comparable_v<non_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_inequality_comparable_v<lte>);
  STATIC_ASSERT_FALSE(arene::base::is_inequality_comparable_v<ne, std::string>);
  STATIC_ASSERT_FALSE(arene::base::is_inequality_comparable_v<throwing_lte>);
  STATIC_ASSERT_FALSE(arene::base::is_inequality_comparable_v<throwing_ne, std::string>);
}

/// @test `is_inequality_comparable_v` evaluates to `true` when the parameter types can be compared using the inequality
/// operator
TEST(IsInEqualityComparable, TrueForTypesWithInequalityOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<ne>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<ne, double>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<throwing_ne>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<throwing_ne, double>);
}

/// @test `is_nothrow_less_than_comparable_v` evaluates to `false` when the parameter types can be compared using the
/// less-than operator
TEST(IsNothrowLessThanComparable, FalseForTypesWithoutLessThanOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<non_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<gte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<less_than_comparable const>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<int, less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<less_than_comparable, std::string>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<throwing_greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<throwing_gte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<throwing_less_than_comparable const>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<int, throwing_less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<throwing_less_than_comparable, std::string>);
}

/// @test `is_nothrow_less_than_comparable_v` evaluates to `true` when the parameter types can be compared using the
/// less-than operator, and that comparison is `noexcept`, `false` otherwise
TEST(IsNothrowLessThanComparable, TrueForTypesWithLessThanOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_less_than_comparable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_less_than_comparable_v<std::string>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_less_than_comparable_v<less_than_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_less_than_comparable_v<less_than_comparable const, int>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_less_than_comparable_v<std::string, less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<throwing_less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<throwing_less_than_comparable const, int>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_comparable_v<std::string, throwing_less_than_comparable>);
}

/// @test `is_nothrow_less_than_or_equal_comparable_v` evaluates to `false` when the parameter types cannot be compared
/// using the less-than-or-equal operator
TEST(IsNothrowLessThanOrEqualComparable, FalseForTypesWithoutLessThanOrEqualOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_or_equal_comparable_v<non_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_or_equal_comparable_v<less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_or_equal_comparable_v<greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_or_equal_comparable_v<int, lte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_or_equal_comparable_v<lte, std::string>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_or_equal_comparable_v<throwing_less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_or_equal_comparable_v<throwing_greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_or_equal_comparable_v<int, throwing_lte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_or_equal_comparable_v<throwing_lte, std::string>);
}

/// @test `is_nothrow_less_than_or_equal_comparable_v` evaluates to `true` when the parameter types can be compared
/// using the less-than-or-equal operator, and that comparison is `noexcept`, `false` otherwise.
TEST(IsNothrowLessThanOrEqualComparable, TrueForTypesWithLessThanOrEqualOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_less_than_or_equal_comparable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_less_than_or_equal_comparable_v<lte>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_less_than_or_equal_comparable_v<std::string, lte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_or_equal_comparable_v<throwing_lte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_less_than_or_equal_comparable_v<std::string, throwing_lte>);
}

/// @test `is_nothrow_greater_than_comparable_v` evaluates to `false` when the parameter types cannot be compared using
/// the greater-than operator or when the parameters may throw
TEST(IsNothrowGreaterThanComparable, FalseForTypesWithoutGreaterThanOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_comparable_v<non_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_comparable_v<int, greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_comparable_v<less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_comparable_v<gte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_comparable_v<int, throwing_greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_comparable_v<throwing_less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_comparable_v<throwing_gte>);
}

/// @test `is_nothrow_greater_than_comparable_v` evaluates to `true` when the parameter types can be compared
/// using the greater-than operator, and that comparison is `noexcept`, `false` otherwise.
TEST(IsNothrowGreaterThanComparable, TrueForTypesWithGreaterThanOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_greater_than_comparable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_greater_than_comparable_v<greater_than_comparable>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_greater_than_comparable_v<greater_than_comparable, int>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_comparable_v<throwing_greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_comparable_v<throwing_greater_than_comparable, int>);
}

/// @test `is_nothrow_greater_than_or_equal_comparable_v` evaluates to `false` when the parameter types cannot be
/// compared using the greater-than-or-equal operator
TEST(IsNothrowGreaterThanOrEqualComparable, FalseForTypesWithoutGreaterThanOrEqualOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<non_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<std::string, gte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<gte, int>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<less_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<std::string, throwing_gte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<throwing_gte, int>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<throwing_greater_than_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<throwing_less_than_comparable>);
}

/// @test `is_nothrow_greater_than_or_equal_comparable_v` evaluates to `true` when the parameter types can be compared
/// using the greater-than-or-equal operator, and that comparison is `noexcept`, `false` otherwise.
TEST(IsNothrowGreaterThanOrEqualComparable, TrueForTypesWithGreaterThanOrEqualOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<gte>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<gte, std::string>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<throwing_gte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_greater_than_or_equal_comparable_v<throwing_gte, std::string>);
}

/// @test `is_nothrow_equality_comparable_v` evaluates to `false` when the parameter types cannot be compared using the
/// equality operator or when the parameters throw
TEST(IsNothrowEqualityComparable, FalseForTypesWithoutEqualityOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_equality_comparable_v<non_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_equality_comparable_v<lte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_equality_comparable_v<lte, eq>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_equality_comparable_v<throwing_lte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_equality_comparable_v<throwing_lte, throwing_eq>);
}

/// @test `is_nothrow_equality_comparable_v` evaluates to `true` when the parameter types can be compared using the
/// equality operator, and that comparison is `noexcept`, `false` otherwise
TEST(IsNothrowEqualityComparable, TrueForTypesWithEqualityOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_equality_comparable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_equality_comparable_v<std::string>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_equality_comparable_v<eq>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_equality_comparable_v<eq, lte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_equality_comparable_v<throwing_eq>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_equality_comparable_v<throwing_eq, throwing_lte>);
}

/// @test `is_nothrow_inequality_comparable_v` evaluates to `false` when the parameter types cannot be compared using
/// the inequality operator or when the parameters may throw
TEST(IsNothrowInEqualityComparable, FalseForTypesWithoutInequalityOperatorBetweenTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_inequality_comparable_v<non_comparable>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_inequality_comparable_v<lte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_inequality_comparable_v<ne, std::string>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_inequality_comparable_v<throwing_lte>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_inequality_comparable_v<throwing_ne, std::string>);
}

/// @test `is_nothrow_inequality_comparable_v` evaluates to `true` when the parameter types can be compared using the
/// inequality operator, and that comparison is `noexcept`, `false` otherwise
TEST(IsNothrowInEqualityComparable, TrueForTypesWithInequalityOperatorBetweenTypes) {
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_inequality_comparable_v<int>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_inequality_comparable_v<ne>);
  STATIC_ASSERT_TRUE(arene::base::is_nothrow_inequality_comparable_v<ne, double>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_inequality_comparable_v<throwing_ne>);
  STATIC_ASSERT_FALSE(arene::base::is_nothrow_inequality_comparable_v<throwing_ne, double>);
}

}  // namespace
