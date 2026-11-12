// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "arene/base/functional/detail/max_fn.hpp"

#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/functional/detail/operator_less.hpp"
#include "arene/base/math/detail/less_nan_highest.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/bitwise_uniform_distribution.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/testing/nan_rejecting_distribution.hpp"
#include "arene/base/testing/property_assertions.hpp"
#include "arene/base/testing/property_macros.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/utility/as_const.hpp"
#include "testlibs/utilities/configurable_value.hpp"

// NaN draws are rejected in the distributionas NaN cases are tested explicitly.
/// @cond
template <>
constexpr auto arene::base::testing::default_distribution<float> =
    arene::base::testing::nan_rejecting_distribution<arene::base::testing::bitwise_uniform_distribution<float>>{};
template <>
constexpr auto arene::base::testing::default_distribution<double> =
    arene::base::testing::nan_rejecting_distribution<arene::base::testing::bitwise_uniform_distribution<double>>{};
/// @endcond

namespace {

template <class Value, class Compare>
struct test_case {
  using value_type = Value;
  using compare_type = Compare;
};

template <class T>
struct MaxFn : testing::Test {};

using arene::base::functional_detail::operator_less;
using arene::base::math_detail::less_nan_highest;

using test_types = ::testing::Types<               //
    test_case<int, decltype(operator_less)>,       //
    test_case<int, std::less<>>,                   //
    test_case<int, decltype(less_nan_highest)>,    //
    test_case<float, decltype(operator_less)>,     //
    test_case<float, std::less<>>,                 //
    test_case<float, decltype(less_nan_highest)>,  //
    test_case<double, decltype(operator_less)>,    //
    test_case<double, std::less<>>,                //
    test_case<double, decltype(less_nan_highest)>  //
    >;

TYPED_TEST_SUITE(MaxFn, test_types, );

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "No arithmetic is performed in these tests so we expect an exact match");

/// @test max_fn returns the larger of two unequivalent values
ARENE_TYPED_PROPERTY(MaxFn, ReturnsLarger, (typename TypeParam::value_type lhs, typename TypeParam::value_type rhs)) {
  ARENE_PROP_PRE(lhs != rhs);

  using compare_type = typename TypeParam::compare_type;
  auto const max = arene::base::functional_detail::max_fn(compare_type{});

  if (&lhs == &max(lhs, rhs)) {
    ARENE_PROP_ASSERT(!compare_type{}(lhs, rhs));
  } else {
    ARENE_PROP_ASSERT(!compare_type{}(rhs, lhs));  // NOLINT(readability-suspicious-call-argument)
  }
}

/// @test max_fn returns the first argument if arguments are equivalent
ARENE_TYPED_PROPERTY(MaxFn, ReturnsFirstIfEquivalent, (typename TypeParam::value_type value)) {
  using compare_type = typename TypeParam::compare_type;
  auto const max = arene::base::functional_detail::max_fn(compare_type{});

  auto const lhs = value;
  auto const rhs = value;

  ARENE_PROP_ASSERT(&lhs == &max(lhs, rhs));
}

/// @test max_fn preserves the return type as T const&
TYPED_TEST(MaxFn, ReturnTypeIsConstReference) {
  using compare_type = typename TypeParam::compare_type;
  using value_type = typename TypeParam::value_type;
  auto const max = arene::base::functional_detail::max_fn(compare_type{});

  ::testing::StaticAssertTypeEq<value_type const&, decltype(max(value_type{}, value_type{}))>();
}

/// @test max_fn is usable in a constant expression
CONSTEXPR_TYPED_TEST(MaxFn, IsConstexpr) {
  using compare_type = typename TypeParam::compare_type;
  auto const max = arene::base::functional_detail::max_fn(compare_type{});

  auto const value = typename TypeParam::value_type{};

  CONSTEXPR_ASSERT(value == max(value, value));
}

/// @test max_fn works with less_nan_highest, ordering NaN after all other
/// non-NaN values
TEST(MaxFn, WorksWithLessNanHighest) {
  auto const max = arene::base::functional_detail::max_fn(less_nan_highest);

  auto const nan = std::numeric_limits<double>::quiet_NaN();
  auto const inf = std::numeric_limits<double>::infinity();

  EXPECT_TRUE(arene::base::isnan(max(nan, nan)));
  EXPECT_TRUE(arene::base::isnan(max(-nan, -nan)));

  EXPECT_TRUE(arene::base::isnan(max(-nan, nan)));
  EXPECT_TRUE(arene::base::isnan(max(nan, -nan)));

  EXPECT_TRUE(arene::base::isnan(max(0., nan)));
  EXPECT_TRUE(arene::base::isnan(max(nan, 0.)));
  EXPECT_TRUE(arene::base::isnan(max(0., -nan)));
  EXPECT_TRUE(arene::base::isnan(max(-nan, 0.)));

  EXPECT_TRUE(arene::base::isnan(max(inf, nan)));
  EXPECT_TRUE(arene::base::isnan(max(nan, inf)));
  EXPECT_TRUE(arene::base::isnan(max(inf, -nan)));
  EXPECT_TRUE(arene::base::isnan(max(-nan, inf)));

  EXPECT_TRUE(arene::base::isnan(max(-inf, nan)));
  EXPECT_TRUE(arene::base::isnan(max(nan, -inf)));
  EXPECT_TRUE(arene::base::isnan(max(-inf, -nan)));
  EXPECT_TRUE(arene::base::isnan(max(-nan, -inf)));
}

/// @test max_fn propagates noexcept of the underlying Compare
TEST(MaxFn, Noexcept) {
  using arene::base::is_invocable_v;
  using arene::base::is_nothrow_invocable_v;

  auto const max = arene::base::functional_detail::max_fn(operator_less);

  ASSERT_TRUE((is_invocable_v<decltype(max), int, int>));
  ASSERT_TRUE((is_nothrow_invocable_v<decltype(max), int, int>));

  using throws_on_less = testing::configurable_value<int, testing::throws_on::less>;
  ASSERT_TRUE((is_invocable_v<decltype(max), throws_on_less, throws_on_less>));
  ASSERT_FALSE((is_nothrow_invocable_v<decltype(max), throws_on_less, throws_on_less>));

  // operator> isn't used
  using throws_on_greater = testing::configurable_value<int, testing::throws_on::greater>;
  ASSERT_TRUE((is_invocable_v<decltype(max), throws_on_greater, throws_on_greater>));
  ASSERT_TRUE((is_nothrow_invocable_v<decltype(max), throws_on_greater, throws_on_greater>));
}

ARENE_IGNORE_END();

/// @test max_fn works with references
TEST(MaxFn, WithReferences) {
  struct my_comp_t {
    auto operator()(int, int) -> std::true_type { return {}; }
    auto operator()(int, int) const -> std::false_type { return {}; }
  };

  auto my_comp = my_comp_t{};

  auto ref_max = arene::base::functional_detail::max_fn<my_comp_t&>(my_comp);
  auto cref_max = arene::base::functional_detail::max_fn<my_comp_t const&>(my_comp);

  auto lhs = int{};
  auto rhs = int{};

  // non-const overload of 'my_comp_t' says lhs is less than rhs
  EXPECT_EQ(&rhs, &ref_max(lhs, rhs));
  // const overload of 'my_comp_t' says lhs is *not* less than rhs
  EXPECT_EQ(&lhs, &cref_max(lhs, rhs));

  using arene::base::as_const;
  // changing the 'const' of the max_fn doesn't propagate to the compare since
  // the compare is specified as a reference
  EXPECT_EQ(&rhs, &as_const(ref_max)(lhs, rhs));
  EXPECT_EQ(&lhs, &as_const(cref_max)(lhs, rhs));
}

}  // namespace
