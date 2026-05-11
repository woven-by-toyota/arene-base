// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mdspan/detail/checked_math.hpp"

#include <gtest/gtest.h>

#include "arene/base/mdspan/tests/test_index_types.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"

namespace {

using arene::base::mdspan_detail::checked_math_result;
using arene::base::mdspan_detail::checked_multiplies;
using arene::base::mdspan_detail::checked_plus;

template <class IndexType>
struct MdspanDetailCheckedMath : testing::Test {};

TYPED_TEST_SUITE(MdspanDetailCheckedMath, test::index_types, );

/// @test adding values where either has already overflowed has a result that
/// has also overflowed
TYPED_TEST(MdspanDetailCheckedMath, PlusAlreadyOverflowed) {
  using index_type = TypeParam;
  using result = checked_math_result<index_type>;
  constexpr auto plus = checked_plus<index_type>;

  EXPECT_TRUE(plus(result{{}, true}, result{{}, false}).overflowed);
  EXPECT_TRUE(plus(result{{}, false}, result{{}, true}).overflowed);
  EXPECT_TRUE(plus(result{{}, true}, result{{}, true}).overflowed);
}

/// @test adding values that would overflow returns a result with overflowed
/// true
TYPED_TEST(MdspanDetailCheckedMath, PlusOverflows) {
  using index_type = TypeParam;
  constexpr auto plus = checked_plus<index_type>;

  EXPECT_TRUE(plus(std::numeric_limits<index_type>::max(), index_type{2}).overflowed);
  EXPECT_TRUE(plus(index_type{2}, std::numeric_limits<index_type>::max()).overflowed);
}

/// @test adding values that would normally cause UB does not
CONSTEXPR_TEST(MdspanDetailCheckedMath, PlusNoSignedOverflowUB) {
  constexpr auto plus = checked_plus<int>;

  CONSTEXPR_ASSERT(plus(std::numeric_limits<int>::max(), 2).overflowed);
  CONSTEXPR_ASSERT(plus(2, std::numeric_limits<int>::max()).overflowed);
}

/// @test adding values that don't overflow returns the same result as adding
/// the underlying values
CONSTEXPR_TEST(MdspanDetailCheckedMath, PlusResultSameAsUnderlying) {
  constexpr auto plus = checked_plus<int>;

  constexpr auto max_value = std::numeric_limits<int>::max();

  CONSTEXPR_ASSERT((0 + 0) == plus(0, 0).value);
  CONSTEXPR_ASSERT((0 + 2) == plus(0, 2).value);
  CONSTEXPR_ASSERT((2 + 0) == plus(2, 0).value);
  CONSTEXPR_ASSERT((1 + 2) == plus(1, 2).value);
  CONSTEXPR_ASSERT((2 + 1) == plus(2, 1).value);
  CONSTEXPR_ASSERT((2 + 1) == plus(2, 1).value);
  CONSTEXPR_ASSERT((0 + max_value) == plus(0, max_value).value);
  CONSTEXPR_ASSERT((max_value + 0) == plus(max_value, 0).value);
}

/// @test multipliying non-zero values where either has already overflowed has a
/// result that has also overflowed
TYPED_TEST(MdspanDetailCheckedMath, MultipliesAlreadyOverflowed) {
  using index_type = TypeParam;
  using result = checked_math_result<index_type>;
  constexpr auto multiplies = checked_multiplies<index_type>;

  EXPECT_TRUE(multiplies(result{{}, true}, index_type{1}).overflowed);
  EXPECT_TRUE(multiplies(index_type{1}, result{{}, true}).overflowed);
  EXPECT_TRUE(multiplies(result{{}, true}, result{{}, true}).overflowed);
}

/// @test multipliying a zero value by an overflowed value returns zero
TYPED_TEST(MdspanDetailCheckedMath, MultipliesByZeroResetsOverflow) {
  using index_type = TypeParam;
  using result = checked_math_result<index_type>;
  constexpr auto multiplies = checked_multiplies<index_type>;

  EXPECT_EQ(multiplies(result{{}, true}, result{}), index_type{});
  EXPECT_EQ(multiplies(result{index_type{1}, true}, result{}), index_type{});
  EXPECT_EQ(multiplies(result{}, result{{}, true}), index_type{});
  EXPECT_EQ(multiplies(result{}, result{index_type{1}, true}), index_type{});
}

/// @test multiplying values that would overflow returns a result with overflowed
/// true
TYPED_TEST(MdspanDetailCheckedMath, MultipliesOverflows) {
  using index_type = TypeParam;
  constexpr auto multiplies = checked_multiplies<index_type>;

  EXPECT_TRUE(multiplies(std::numeric_limits<index_type>::max(), index_type{2}).overflowed);
  EXPECT_TRUE(multiplies(index_type{2}, std::numeric_limits<index_type>::max()).overflowed);
}

/// @test multipliying values that would normally cause UB does not
CONSTEXPR_TEST(MdspanDetailCheckedMath, MultipliesNoSignedOverflowUB) {
  constexpr auto multiplies = checked_multiplies<int>;

  CONSTEXPR_ASSERT(multiplies(std::numeric_limits<int>::max(), 2).overflowed);
  CONSTEXPR_ASSERT(multiplies(2, std::numeric_limits<int>::max()).overflowed);
}

/// @test multiplying values that don't overflow returns the same result as
/// multiplying  the underlying values
CONSTEXPR_TEST(MdspanDetailCheckedMath, MultipliesResultSameAsUnderlying) {
  constexpr auto multiplies = checked_multiplies<int>;

  constexpr auto max_value = std::numeric_limits<int>::max();

  CONSTEXPR_ASSERT((0 * 0) == multiplies(0, 0).value);
  CONSTEXPR_ASSERT((0 * 2) == multiplies(0, 2).value);
  CONSTEXPR_ASSERT((2 * 0) == multiplies(2, 0).value);
  CONSTEXPR_ASSERT((1 * 2) == multiplies(1, 2).value);
  CONSTEXPR_ASSERT((2 * 1) == multiplies(2, 1).value);
  CONSTEXPR_ASSERT((2 * 1) == multiplies(2, 1).value);
  CONSTEXPR_ASSERT((0 * max_value) == multiplies(0, max_value).value);
  CONSTEXPR_ASSERT((max_value * 0) == multiplies(max_value, 0).value);
  CONSTEXPR_ASSERT((1 * max_value) == multiplies(1, max_value).value);
  CONSTEXPR_ASSERT((max_value * 1) == multiplies(max_value, 1).value);
}

}  // namespace
