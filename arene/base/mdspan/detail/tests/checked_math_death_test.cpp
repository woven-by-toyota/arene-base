// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/mdspan/detail/checked_math.hpp"
#include "arene/base/mdspan/tests/test_index_types.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"

namespace {

template <class IndexType>
struct MdspanDetailCheckedMathDeathTest : testing::Test {};

TYPED_TEST_SUITE(MdspanDetailCheckedMathDeathTest, test::index_types, );

/// @test adding values where either is negative is a precondition violation
TYPED_TEST(MdspanDetailCheckedMathDeathTest, CheckedPlusWithNegativeValues) {
  using index_type = TypeParam;

  if (std::is_signed<index_type>::value) {
    constexpr auto minus_one = static_cast<index_type>(-1);
    constexpr auto plus = arene::base::mdspan_detail::checked_plus<index_type>;
    ASSERT_DEATH(plus(index_type{}, minus_one), "Precondition");
    ASSERT_DEATH(plus(minus_one, index_type{}), "Precondition");
    ASSERT_DEATH(plus(minus_one, minus_one), "Precondition");
  }
}

/// @test multipliying values where either is negative and no value is zero is a
/// precondition violation
TYPED_TEST(MdspanDetailCheckedMathDeathTest, CheckedMultipliesWithNegativeValues) {
  using index_type = TypeParam;

  if (std::is_signed<index_type>::value) {
    constexpr auto minus_one = static_cast<index_type>(-1);
    constexpr auto multiplies = arene::base::mdspan_detail::checked_multiplies<index_type>;
    ASSERT_DEATH(multiplies(index_type{1}, minus_one), "Precondition");
    ASSERT_DEATH(multiplies(minus_one, index_type{1}), "Precondition");
    ASSERT_DEATH(multiplies(minus_one, minus_one), "Precondition");
  }
}

}  // namespace
