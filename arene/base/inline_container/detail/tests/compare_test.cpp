// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/inline_container/detail/compare.hpp"

#include <functional>

#include <gtest/gtest.h>

#include "arene/base/compare.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {
using ::arene::base::strong_ordering;
using ::arene::base::inline_container::detail::compare_wrapper;

/// @brief A throwing binary predicate
class throwing_less {
  template <typename Lhs, typename Rhs>
  constexpr auto operator()(Lhs const& lhs, Rhs const& rhs) const noexcept(false) -> bool {
    return std::less<>{}(lhs, rhs);
  }
};

/// @brief A throwing three way comparison
class throwing_three_way {
  template <typename Lhs, typename Rhs>
  constexpr auto operator()(Lhs const& lhs, Rhs const& rhs) const noexcept(false) -> strong_ordering {
    return ::arene::base::compare_three_way{}(lhs, rhs);
  }
};

template <typename T>
class CompareTest : public ::testing::Test {};

using comparator_types = ::testing::Types<std::less<>, ::arene::base::compare_three_way>;

TYPED_TEST_SUITE(CompareTest, comparator_types, );

/// @test Invoking the comparator is `noexcept` if the wrapped comparison is `noexcept` for comparing integers
///
/// @tparam TypeParam The wrapped comparator
TYPED_TEST(CompareTest, IsComparisonNoexceptIsTrueIfUnderlyingComparatorIsNoexceptForOperands) {
  using key_type = int;
  using compare = compare_wrapper<TypeParam, key_type>;
  // we use == rather than expect_eq to avoid ODR violations with the runtime expect_eq
  STATIC_ASSERT_TRUE(compare::template comparison_is_noexcept<> == noexcept(TypeParam{}(key_type{}, key_type{})));
  STATIC_ASSERT_TRUE(compare::template comparison_is_noexcept<double> == noexcept(TypeParam{}(key_type{}, double{})));
}

/// @test `key_ordering` is `noexcept` if the underlying comparison is `noexcept`
/// @tparam TypeParam The wrapped comparator
TYPED_TEST(CompareTest, KeyOrderingNoexceptMatchesIsComparisonNoexcept) {
  using key_type = int;
  using compare = compare_wrapper<TypeParam, key_type>;
  // we use == rather than expect_eq to avoid ODR violations with the runtime expect_eq
  STATIC_ASSERT_TRUE(noexcept(compare{}.key_ordering(key_type{}, key_type{})) == compare::template comparison_is_noexcept<>);
  STATIC_ASSERT_TRUE(noexcept(compare{}.key_ordering(key_type{}, double{})) == compare::template comparison_is_noexcept<double>);
}

/// @test `key_ordering` yields the appropriate 3-way comparison result for the operands
/// @tparam TypeParam The wrapped comparator
TYPED_TEST(CompareTest, KeyOrderingProducesComparisonEquivalentToAThreeWayComparisonOfTheOperands) {
  constexpr auto compare = compare_wrapper<TypeParam, int>{};
  STATIC_ASSERT_EQ(compare.key_ordering(1, 10), strong_ordering::less);
  STATIC_ASSERT_EQ(compare.key_ordering(1, 1), strong_ordering::equal);
  STATIC_ASSERT_EQ(compare.key_ordering(10, 1), strong_ordering::greater);

  STATIC_ASSERT_EQ(compare.key_ordering(1, 10.0), strong_ordering::less);
  STATIC_ASSERT_EQ(compare.key_ordering(1, 1.0), strong_ordering::equal);
  STATIC_ASSERT_EQ(compare.key_ordering(10, 1.0), strong_ordering::greater);
}

}  // namespace
