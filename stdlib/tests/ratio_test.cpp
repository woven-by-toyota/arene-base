// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stdlib/include/ratio"

#include "arene/base/contracts.hpp"
#include "arene/base/math/abs.hpp"
#include "arene/base/math/gcd.hpp"
#include "stdlib/include/algorithm"
#include "stdlib/include/tuple"
#include "stdlib/include/type_traits"
#include "stdlib/include/utility"
#include "testlibs/minitest/minitest.hpp"

namespace {

using testing::is_unambiguously_publicly_derived_from_v;

constexpr auto sign(std::intmax_t value) noexcept -> int { return value > 0 ? 1 : (value == 0 ? 0 : -1); }

constexpr auto power_of_ten(unsigned int exponent) noexcept -> std::intmax_t {
  std::intmax_t result{1};

  for (unsigned int i = 0; i < exponent; ++i) {
    result *= 10;
  }
  return result;
}

template <typename T>
class ratio_info;

template <std::intmax_t Num, std::intmax_t Den>
class ratio_info<std::ratio<Num, Den>> {
 public:
  static constexpr std::intmax_t num_param{Num};
  static constexpr std::intmax_t den_param{Den};
  static constexpr std::intmax_t num_member{std::ratio<Num, Den>::num};
  static constexpr std::intmax_t den_member{std::ratio<Num, Den>::den};
};

template <class T>
class Ratio : protected testing::Test {};

// clang-format off
using ratio_args = ::testing::Types<
  std::ratio<1, 1>,
  std::ratio<1, 2>,
  std::ratio<2, 2>,
  std::ratio<2, 3>,
  std::ratio<0, 1>,
  std::ratio<0, 2>,
  std::ratio<0, -1>,
  std::ratio<1, -2>,
  std::ratio<-1, 2>,
  std::ratio<-1, -2>,
  std::ratio<7, 21>,
  std::ratio<7, 22>,
  std::ratio<8, 22>,
  std::ratio<std::numeric_limits<std::intmax_t>::max() - 1, std::numeric_limits<std::intmax_t>::max()>,
  std::ratio<std::numeric_limits<std::intmax_t>::min() + 1, std::numeric_limits<std::intmax_t>::min() + 2>
>;
// clang-format on

TYPED_TEST_SUITE(Ratio, ratio_args, );

/// @test @c ratio can be instantiated and has no non-static data
CONSTEXPR_TYPED_TEST(Ratio, CanInstantiate) {
  TypeParam ratio{};
  std::ignore = ratio;
  static_assert(std::is_empty_v<TypeParam>, "std::ratio instances have no non-static data");
}

/// @test @c ratio always has a numerator and denominator which are relatively prime (the ratio is in lowest terms)
CONSTEXPR_TYPED_TEST(Ratio, RatioIsInLowestTerms) { EXPECT_EQ(arene::base::gcd(TypeParam::num, TypeParam::den), 1); }

/// @test The numerator of a @c ratio matches the combined sign of the parameters
CONSTEXPR_TYPED_TEST(Ratio, NumeratorContainsSignOfParameters) {
  using this_ratio = ratio_info<TypeParam>;

  EXPECT_EQ(sign(TypeParam::num), sign(this_ratio::num_param) * sign(this_ratio::den_param));
}

/// @test The denominator of a @c ratio is always positive
CONSTEXPR_TYPED_TEST(Ratio, DenominatorIsAlwaysPositive) { EXPECT_TRUE(TypeParam::den > 0); }

/// @test Final ratio members of a @c ratio are reduced from its parameters
CONSTEXPR_TYPED_TEST(Ratio, RatioReducedFromParameters) {
  using this_ratio = ratio_info<TypeParam>;

  EXPECT_TRUE(arene::base::abs(this_ratio::num_member) <= arene::base::abs(this_ratio::num_param));
  EXPECT_TRUE(arene::base::abs(this_ratio::den_member) <= arene::base::abs(this_ratio::den_param));

  auto gcd = arene::base::gcd(this_ratio::num_param, this_ratio::den_param);
  EXPECT_EQ(arene::base::abs(this_ratio::num_member) * gcd, arene::base::abs(this_ratio::num_param));
  EXPECT_EQ(arene::base::abs(this_ratio::den_member) * gcd, arene::base::abs(this_ratio::den_param));
}

/// @test The typedef @c ratio<N, D>::type is also in lowest terms, including its template parameters
CONSTEXPR_TYPED_TEST(Ratio, TypedefIsInLowestTerms) {
  // Note: TypeParam and TypeParam::type are not necessarily the same type, but their static members are the same
  EXPECT_EQ(TypeParam::num, TypeParam::type::num);
  EXPECT_EQ(TypeParam::den, TypeParam::type::den);

  using typedef_info = ratio_info<typename TypeParam::type>;
  EXPECT_EQ(typedef_info::num_param, TypeParam::num);
  EXPECT_EQ(typedef_info::den_param, TypeParam::den);
  EXPECT_EQ(arene::base::gcd(typedef_info::num_param, typedef_info::den_param), 1);
}

/// @test The default denominator of a @c ratio is 1
CONSTEXPR_TYPED_TEST(Ratio, DefaultDenominatorIsOne) {
  using automatic = std::ratio<TypeParam::num>;
  using manual = std::ratio<TypeParam::num, 1>;
  testing::StaticAssertTypeEq<automatic, manual>();
}

/// @test Multiplication by the identity gives the original ratio back
CONSTEXPR_TYPED_TEST(Ratio, MultiplyByIdentity) {
  // Have to compare to the TypeParam::type typdef which has its parameters reduced, since ratio ops always reduce.
  testing::StaticAssertTypeEq<std::ratio_multiply<TypeParam, std::ratio<1>>, typename TypeParam::type>();
  testing::StaticAssertTypeEq<std::ratio_divide<TypeParam, std::ratio<1>>, typename TypeParam::type>();
}

/// @test Multiplication by the reciprocal gives the identity
CONDITIONAL_TYPED_TEST(Ratio, MultiplyByReciprocal, TypeParam::num != 0) {
  testing::
      StaticAssertTypeEq<std::ratio_multiply<TypeParam, std::ratio<TypeParam::den, TypeParam::num>>, std::ratio<1>>();
  testing::StaticAssertTypeEq<std::ratio_divide<TypeParam, TypeParam>, std::ratio<1>>();
}

/// @test Addition of the identity gives the original ratio back
CONSTEXPR_TYPED_TEST(Ratio, AddIdentity) {
  // Have to compare to the TypeParam::type typdef which has its parameters reduced, since ratio ops always reduce.
  testing::StaticAssertTypeEq<std::ratio_add<TypeParam, std::ratio<0>>, typename TypeParam::type>();
  testing::StaticAssertTypeEq<std::ratio_subtract<TypeParam, std::ratio<0>>, typename TypeParam::type>();
}

/// @test Addition of the inverse gives the identity and subtraction from the identity gives the inverse
CONSTEXPR_TYPED_TEST(Ratio, AddInverse) {
  using inverse = std::ratio<-TypeParam::num, TypeParam::den>;
  testing::StaticAssertTypeEq<std::ratio_add<TypeParam, inverse>, std::ratio<0>>();
  testing::StaticAssertTypeEq<std::ratio_subtract<TypeParam, TypeParam>, std::ratio<0>>();
  testing::StaticAssertTypeEq<std::ratio_add<std::ratio<0>, inverse>, inverse>();
  testing::StaticAssertTypeEq<std::ratio_subtract<std::ratio<0>, TypeParam>, inverse>();
}

/// @test Various multiplications give the correct results
CONSTEXPR_TEST(RatioUnparameterized, Multiply) {
  constexpr auto max = std::numeric_limits<std::intmax_t>::max();
  testing::StaticAssertTypeEq<std::ratio_multiply<std::ratio<1, 3>, std::ratio<3, 2>>, std::ratio<1, 2>>();
  testing::StaticAssertTypeEq<std::ratio_multiply<std::ratio<1, max>, std::ratio<max, 2>>, std::ratio<1, 2>>();
  testing::StaticAssertTypeEq<std::ratio_multiply<std::ratio<7, max>, std::ratio<max, 3>>, std::ratio<7, 3>>();
}

/// @test Various divisions give the correct results
CONSTEXPR_TEST(RatioUnparameterized, Divide) {
  constexpr auto max = std::numeric_limits<std::intmax_t>::max();
  testing::StaticAssertTypeEq<std::ratio_divide<std::ratio<1, 3>, std::ratio<2, 3>>, std::ratio<1, 2>>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::ratio<1, max>, std::ratio<2, max>>, std::ratio<1, 2>>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::ratio<7, max>, std::ratio<3, max>>, std::ratio<7, 3>>();
}

/// @test Various additions give the correct results
CONSTEXPR_TEST(RatioUnparameterized, Add) {
  constexpr auto max = std::numeric_limits<std::intmax_t>::max();
  testing::StaticAssertTypeEq<std::ratio_add<std::ratio<1, 3>, std::ratio<1, 6>>::type, std::ratio<1, 2>>();
  testing::StaticAssertTypeEq<std::ratio_add<std::ratio<1, max>, std::ratio<1, max>>::type, std::ratio<2, max>>();
  testing::StaticAssertTypeEq<std::ratio_add<std::ratio<max, 4>, std::ratio<max, 4>>::type, std::ratio<max, 2>>();
}

/// @test Various subtractions give the correct results
CONSTEXPR_TEST(RatioUnparameterized, Subtract) {
  constexpr auto max = std::numeric_limits<std::intmax_t>::max();
  testing::StaticAssertTypeEq<std::ratio_subtract<std::ratio<1, 3>, std::ratio<1, 6>>::type, std::ratio<1, 6>>();
  testing::StaticAssertTypeEq<std::ratio_subtract<std::ratio<3, max>, std::ratio<2, max>>::type, std::ratio<1, max>>();
  testing::StaticAssertTypeEq<std::ratio_subtract<std::ratio<max, 4>, std::ratio<max, 8>>::type, std::ratio<max, 8>>();
}

/// @test Comparisons of a @c ratio with itself behave like an ordered field
CONSTEXPR_TYPED_TEST(Ratio, ComparisonsAreReflexive) {
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_equal<TypeParam, TypeParam>, std::true_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_not_equal<TypeParam, TypeParam>, std::false_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_less<TypeParam, TypeParam>, std::false_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_less_equal<TypeParam, TypeParam>, std::true_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_greater<TypeParam, TypeParam>, std::false_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_greater_equal<TypeParam, TypeParam>, std::true_type>);
}

/// @test Comparisons of a @c ratio consider the reduced form of a ratio, not its parameters
CONSTEXPR_TYPED_TEST(Ratio, ComparisonsOperateOnReducedForms) {
  constexpr std::intmax_t multiplier{
      TypeParam::num == 0 ? 1
                          : std::min(
                                std::numeric_limits<std::intmax_t>::max() / arene::base::abs(TypeParam::num),
                                std::numeric_limits<std::intmax_t>::max() / TypeParam::den
                            )
  };

  using mult = std::ratio<multiplier * TypeParam::num, multiplier * TypeParam::den>;

  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_equal<TypeParam, mult>, std::true_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_not_equal<TypeParam, mult>, std::false_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_less<TypeParam, mult>, std::false_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_less_equal<TypeParam, mult>, std::true_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_greater<TypeParam, mult>, std::false_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_greater_equal<TypeParam, mult>, std::true_type>);
}

constexpr auto saturating_add(std::intmax_t left, std::intmax_t right) noexcept -> std::intmax_t {
  ARENE_PRECONDITION(right > 0);
  if (left < 0) {
    return left + right;
  }

  auto increment = std::min(std::numeric_limits<std::intmax_t>::max() - left, right);
  return left + increment;
}

/// @test Comparisons of a @c ratio with a positively shifted one behave like an ordered field
CONSTEXPR_TYPED_TEST(Ratio, ComparisonsWorkForShiftedParameters) {
  constexpr std::intmax_t next_num{saturating_add(TypeParam::num, TypeParam::den)};
  if (next_num == TypeParam::num) {
    return;
  }
  using next = std::ratio<next_num, TypeParam::den>;

  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_equal<TypeParam, next>, std::false_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_not_equal<TypeParam, next>, std::true_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_less<TypeParam, next>, std::true_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_less_equal<TypeParam, next>, std::true_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_greater<TypeParam, next>, std::false_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_greater_equal<TypeParam, next>, std::false_type>);
}

// A second test suite for the comparison tests which have two type parameters
template <class T>
class RatioComparison : protected testing::Test {};

// In all cases, the left side is strictly less than the right side
// clang-format off
using ratio_comparison_pairs = ::testing::Types<
  std::pair<std::ratio<-2>, std::ratio<1>>,
  std::pair<std::ratio<1, 3>, std::ratio<1, 2>>,
  std::pair<std::ratio<-1, 3>, std::ratio<0, 2>>,
  std::pair<std::ratio<0, 2>, std::ratio<1, 3>>,
  std::pair<std::ratio<3, 16>, std::ratio<16, 3>>,
  std::pair<std::ratio<22, 7>,
            std::ratio<std::numeric_limits<std::intmax_t>::max(), 13>>,
  std::pair<std::ratio<std::numeric_limits<std::intmax_t>::min() + 2, 13>,
            std::ratio<17, 662371>>,
  std::pair<std::ratio<std::numeric_limits<std::intmax_t>::max() - 1>,
            std::ratio<std::numeric_limits<std::intmax_t>::max()>>,
  std::pair<std::ratio<std::numeric_limits<std::intmax_t>::min() + 1>,
            std::ratio<std::numeric_limits<std::intmax_t>::min() + 2>>,
  std::pair<std::ratio<std::numeric_limits<std::intmax_t>::min() + 1>,
            std::ratio<std::numeric_limits<std::intmax_t>::max() - 1>>,
  std::pair<std::ratio<std::numeric_limits<std::intmax_t>::max(), 97>,
            std::ratio<std::numeric_limits<std::intmax_t>::max(), 53>>
>;
// clang-format on

TYPED_TEST_SUITE(RatioComparison, ratio_comparison_pairs, );

/// @test Various comparisons give the correct results when the first type is less than the second
CONSTEXPR_TYPED_TEST(RatioComparison, Less) {
  using R1 = typename TypeParam::first_type;
  using R2 = typename TypeParam::second_type;

  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_equal<R1, R2>, std::false_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_not_equal<R1, R2>, std::true_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_less<R1, R2>, std::true_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_less_equal<R1, R2>, std::true_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_greater<R1, R2>, std::false_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_greater_equal<R1, R2>, std::false_type>);
}

/// @test Various comparisons give the correct results when the first type is greater than the second
CONSTEXPR_TYPED_TEST(RatioComparison, Greater) {
  using R1 = typename TypeParam::first_type;
  using R2 = typename TypeParam::second_type;

  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_equal<R2, R1>, std::false_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_not_equal<R2, R1>, std::true_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_less<R2, R1>, std::false_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_less_equal<R2, R1>, std::false_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_greater<R2, R1>, std::true_type>);
  EXPECT_TRUE(is_unambiguously_publicly_derived_from_v<std::ratio_greater_equal<R2, R1>, std::true_type>);
}

/// @test The helper functions for comparison work correctly
CONSTEXPR_TYPED_TEST(RatioComparison, HelperFunctions) {
  // This test is needed because we don't get coverage for functions that are only called during template instantiation
  using R1 = typename TypeParam::first_type;
  using R2 = typename TypeParam::second_type;

  EXPECT_TRUE(std::ratio_detail::fraction_less({R1::num, R1::den}, {R2::num, R2::den}));
  EXPECT_FALSE(std::ratio_detail::fraction_less({R2::num, R2::den}, {R1::num, R1::den}));
}

CONSTEXPR_TEST(RatioUnparameterized, SiTypedefValues) {
  EXPECT_EQ(std::atto::den, power_of_ten(18));
  EXPECT_EQ(std::femto::den, power_of_ten(15));
  EXPECT_EQ(std::pico::den, power_of_ten(12));
  EXPECT_EQ(std::nano::den, power_of_ten(9));
  EXPECT_EQ(std::micro::den, power_of_ten(6));
  EXPECT_EQ(std::milli::den, power_of_ten(3));
  EXPECT_EQ(std::centi::den, power_of_ten(2));
  EXPECT_EQ(std::deci::den, power_of_ten(1));
  EXPECT_EQ(std::deca::num, power_of_ten(1));
  EXPECT_EQ(std::hecto::num, power_of_ten(2));
  EXPECT_EQ(std::kilo::num, power_of_ten(3));
  EXPECT_EQ(std::mega::num, power_of_ten(6));
  EXPECT_EQ(std::giga::num, power_of_ten(9));
  EXPECT_EQ(std::tera::num, power_of_ten(12));
  EXPECT_EQ(std::peta::num, power_of_ten(15));
  EXPECT_EQ(std::exa::num, power_of_ten(18));
}

/// @test The SI typedefs have the correct relationships to each other
CONSTEXPR_TEST(RatioUnparameterized, SiTypedefRatios) {
  testing::StaticAssertTypeEq<std::ratio_divide<std::exa, std::peta>, std::kilo>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::peta, std::tera>, std::kilo>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::tera, std::giga>, std::kilo>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::giga, std::mega>, std::kilo>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::mega, std::kilo>, std::kilo>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::kilo, std::hecto>, std::deca>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::hecto, std::deca>, std::deca>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::deca, std::deci>, std::hecto>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::deci, std::centi>, std::deca>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::centi, std::milli>, std::deca>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::milli, std::micro>, std::kilo>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::micro, std::nano>, std::kilo>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::nano, std::pico>, std::kilo>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::pico, std::femto>, std::kilo>();
  testing::StaticAssertTypeEq<std::ratio_divide<std::femto, std::atto>, std::kilo>();
}

}  // namespace
