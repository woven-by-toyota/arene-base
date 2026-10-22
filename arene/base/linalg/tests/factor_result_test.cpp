// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/linalg/factor_result.hpp"

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/is_implicitly_constructible.hpp"

namespace {

using arene::base::linalg::factor_result;

/// @brief The largest pivot index a @c factor_result can hold
constexpr std::size_t largest_pivot_index{std::numeric_limits<std::size_t>::max() - 1U};

/// @test Given a default constructed result, when it is queried, then it denotes success without a pivot index.
CONSTEXPR_TEST(FactorResultTest, DefaultConstructedResultDenotesSuccess) {
  factor_result const result{};

  CONSTEXPR_ASSERT(static_cast<bool>(result));
  CONSTEXPR_ASSERT_FALSE(result.has_pivot_index());
}

/// @test Given a result constructed from a pivot index, when it is queried, then it denotes failure at that pivot.
CONSTEXPR_TEST(FactorResultTest, ResultConstructedFromIndexDenotesFailureAtThatIndex) {
  factor_result const result{7U};

  CONSTEXPR_ASSERT_FALSE(static_cast<bool>(result));
  CONSTEXPR_ASSERT(result.has_pivot_index());
  CONSTEXPR_ASSERT_EQ(result.pivot_index(), 7U);
}

/// @test Given the first and the largest representable pivot index, when a result holds either, then it is reported
/// unchanged.
CONSTEXPR_TEST(FactorResultTest, ExtremePivotIndicesAreRepresented) {
  factor_result const first_pivot{0U};
  factor_result const last_pivot{largest_pivot_index};

  CONSTEXPR_ASSERT(first_pivot.has_pivot_index());
  CONSTEXPR_ASSERT_EQ(first_pivot.pivot_index(), 0U);
  CONSTEXPR_ASSERT(last_pivot.has_pivot_index());
  CONSTEXPR_ASSERT_EQ(last_pivot.pivot_index(), largest_pivot_index);
}

/// @test Given a pivot index, when it is used where a result is expected, then it does not convert implicitly.
CONSTEXPR_TEST(FactorResultTest, PivotIndexDoesNotConvertImplicitlyToResult) {
  CONSTEXPR_ASSERT_FALSE((arene::base::is_implicitly_constructible_v<factor_result, std::size_t>));
}

/// @test Given a result, when it is used where a @c bool is expected, then it does not convert implicitly.
CONSTEXPR_TEST(FactorResultTest, ResultDoesNotConvertImplicitlyToBool) {
  CONSTEXPR_ASSERT_FALSE((std::is_convertible<factor_result, bool>::value));
}

/// @test Given a copy of a result, when the copy is queried, then it reports the same pivot index state.
CONSTEXPR_TEST(FactorResultTest, CopiesReportTheSameState) {
  factor_result const success{};
  factor_result const failure{2U};
  factor_result const success_copy{success};
  factor_result const failure_copy{failure};

  CONSTEXPR_ASSERT_FALSE(success_copy.has_pivot_index());
  CONSTEXPR_ASSERT(failure_copy.has_pivot_index());
  CONSTEXPR_ASSERT_EQ(failure_copy.pivot_index(), failure.pivot_index());
}

}  // namespace
