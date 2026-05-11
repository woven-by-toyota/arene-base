// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <gtest/gtest.h>

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/ratio.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/units.hpp"

namespace {

/// @test A scaled version of a quantity unit is a different type, but is still a valid unit for the base quantity kind
/// of the original unit
TEST(ScaledUnits, AScaledUnitIsAPossibleUnitForTheBaseKindOfTheOriginalUnit) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  using scaled = arene::base::scaled_unit<my_unit, std::kilo>;

  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<scaled, my_kind>);

  STATIC_ASSERT_FALSE(std::is_same<scaled, my_unit>::value);
}

/// @test Given @c SU which is <c>scaled_unit<U,Scale></c>, @c SU::scale_factor is @c Scale
TEST(ScaledUnits, CanGetScaledUnitScaleFactor) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  using scaled = arene::base::scaled_unit<my_unit, std::kilo>;

  ::testing::StaticAssertTypeEq<scaled::scale_factor, std::kilo>();
}

/// @test <c>scaled_unit<scaled_unit<BaseUnit,Scale1>,Scale2></c> is the same as
/// <c>scaled_unit<BaseUnit,std::ratio_multiple<Scale1,Scale2>></c> if @c Scale1 and @c Scale2 are ratios,
TEST(ScaledUnits, ScalingScaledUnitCombinesScales) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  using scaled1 = arene::base::scaled_unit<my_unit, std::kilo>;
  using scaled2 = arene::base::scaled_unit<scaled1, std::ratio<4, 3>>;

  ::testing::StaticAssertTypeEq<scaled2::scale_factor, std::ratio<4000, 3>>();
  ::testing::StaticAssertTypeEq<scaled2, arene::base::scaled_unit<my_unit, std::ratio<4000, 3>>>();
}

/// @test <c>scaled_unit<BaseUnit,Scale></c> is an alias for @c BaseUnit if @c Scale is a @c std::ratio with equal
/// numerator and denominator
TEST(ScaledUnits, ScaleOfOneDisappears) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  using scaled1 = arene::base::scaled_unit<my_unit, std::kilo>;
  using scaled2 = arene::base::scaled_unit<scaled1, std::milli>;

  ::testing::StaticAssertTypeEq<scaled2, my_unit>();
  ::testing::StaticAssertTypeEq<my_unit, arene::base::scaled_unit<my_unit, std::ratio<1, 1>>>();
  ::testing::StaticAssertTypeEq<my_unit, arene::base::scaled_unit<my_unit, std::ratio<2, 2>>>();
  ::testing::StaticAssertTypeEq<scaled1, arene::base::scaled_unit<scaled1, std::ratio<2, 2>>>();
}

/// @test Combining @c scaled_unit instances with @c combine_kinds_t does not multiply the scales, the different @c
/// scaled_unit instances are kept distinct
TEST(ScaledUnits, CombiningScaledUnitInstancesDoesNotCombineTheScales) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  using scaled = arene::base::scaled_unit<my_unit, std::kilo>;
  using scaled2 = arene::base::scaled_unit<my_unit, std::mega>;

  ::testing::StaticAssertTypeEq<arene::base::combine_kinds_t<scaled, scaled>, scaled::with_exponent<2>>();
  STATIC_ASSERT_FALSE(std::is_same<
                      arene::base::combine_kinds_t<scaled, scaled2>,
                      arene::base::scaled_unit<my_unit::with_exponent<2>, std::giga>>::value);
  STATIC_ASSERT_FALSE(std::is_same<
                      arene::base::combine_kinds_t<scaled, my_unit>,
                      arene::base::scaled_unit<my_unit::with_exponent<2>, std::kilo>>::value);
}

class three_point_five {
 public:
  static constexpr double value{3.5};
};

class six {
 public:
  static constexpr double value{6.};
};

/// @test Can scale by a class with a @c value constant
TEST(ScaledUnits, CanScaleByValueWrapper) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  using scaled = arene::base::scaled_unit<my_unit, three_point_five>;

  ::testing::StaticAssertTypeEq<scaled::scale_factor, three_point_five>();
}

/// @test Can scale by two classes with a @c value constant
TEST(ScaledUnits, CanScaleByValueWrapperTwice) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  using scaled = arene::base::scaled_unit<my_unit, three_point_five>;
  using scaled2 = arene::base::scaled_unit<scaled, six>;

  STATIC_ASSERT_NEAR(scaled2::scale_factor::value, 6 * 3.5, 0.0000001);
}

/// @test Can scale by a class with a @c value constant and a ratio
TEST(ScaledUnits, CanScaleByValueWrapperAndRatio) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  using scaled = arene::base::scaled_unit<my_unit, three_point_five>;
  using scaled2 = arene::base::scaled_unit<scaled, std::kilo>;

  STATIC_ASSERT_NEAR(scaled2::scale_factor::value, 3500., 0.0000001);
}

/// @test Can cancel ratios even when also scaled by a class with a @c value
TEST(ScaledUnits, CanCancelRatiosWhenScalingByValueWrapperAndRatio) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  using scaled = arene::base::scaled_unit<my_unit, three_point_five>;
  using scaled2 = arene::base::scaled_unit<scaled, std::kilo>;
  using scaled3 = arene::base::scaled_unit<scaled2, std::milli>;

  using scaled4 = arene::base::scaled_unit<my_unit, std::kilo>;
  using scaled5 = arene::base::scaled_unit<scaled4, three_point_five>;
  using scaled6 = arene::base::scaled_unit<scaled5, std::milli>;

  ::testing::StaticAssertTypeEq<scaled3::scale_factor, three_point_five>();
  ::testing::StaticAssertTypeEq<scaled6::scale_factor, three_point_five>();
}

/// @test Can cancel ratios even when also scaled by multiple classes with a @c value
TEST(ScaledUnits, CanCancelRatiosWhenScalingByMultipleValueWrapperAndRatio) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  using scaled = arene::base::scaled_unit<my_unit, three_point_five>;
  using scaled2 = arene::base::scaled_unit<scaled, std::kilo>;
  using scaled3 = arene::base::scaled_unit<scaled2, six>;
  using scaled4 = arene::base::scaled_unit<scaled3, std::milli>;

  using scaled5 = arene::base::scaled_unit<my_unit, std::kilo>;
  using scaled6 = arene::base::scaled_unit<scaled5, three_point_five>;
  using scaled7 = arene::base::scaled_unit<scaled6, six>;
  using scaled8 = arene::base::scaled_unit<scaled7, std::milli>;

  using scaled_values_only = arene::base::scaled_unit<scaled, six>;

  ::testing::StaticAssertTypeEq<scaled4::scale_factor, scaled_values_only::scale_factor>();
  ::testing::StaticAssertTypeEq<scaled8::scale_factor, scaled_values_only::scale_factor>();
}

/// @test Can invert scales
TEST(ScaledUnits, CanInvertScales) {
  using inverted = arene::base::inverse_unit_scale_t<three_point_five>;
  STATIC_ASSERT_NEAR(inverted::value * three_point_five::value, 1., 0.0000001);

  ::testing::StaticAssertTypeEq<arene::base::inverse_unit_scale_t<inverted>, three_point_five>();
  ::testing::StaticAssertTypeEq<arene::base::inverse_unit_scale_t<std::kilo>, std::milli>();
  ::testing::StaticAssertTypeEq<arene::base::inverse_unit_scale_t<std::ratio<4, 3>>, std::ratio<3, 4>>();
}

/// @test Can cancel inverse class scales
TEST(ScaledUnits, CanCancelInverseClassScales) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  using scaled = arene::base::scaled_unit<my_unit, three_point_five>;
  using scaled2 = arene::base::scaled_unit<scaled, six>;
  using scaled3 = arene::base::scaled_unit<scaled2, arene::base::inverse_unit_scale_t<six>>;

  using scaled4 = arene::base::scaled_unit<my_unit, six>;
  using scaled5 = arene::base::scaled_unit<scaled4, three_point_five>;
  using scaled6 = arene::base::scaled_unit<scaled5, arene::base::inverse_unit_scale_t<six>>;

  ::testing::StaticAssertTypeEq<scaled3::scale_factor, three_point_five>();
  ::testing::StaticAssertTypeEq<scaled6::scale_factor, three_point_five>();
}

/// @test Can cancel inverse class scales in reverse
TEST(ScaledUnits, CanCancelInverseClassScalesInReverse) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  using scaled = arene::base::scaled_unit<my_unit, three_point_five>;
  using scaled2 = arene::base::scaled_unit<scaled, arene::base::inverse_unit_scale_t<six>>;
  using scaled3 = arene::base::scaled_unit<scaled2, six>;

  using scaled4 = arene::base::scaled_unit<my_unit, arene::base::inverse_unit_scale_t<six>>;
  using scaled5 = arene::base::scaled_unit<scaled4, three_point_five>;
  using scaled6 = arene::base::scaled_unit<scaled5, six>;

  ::testing::StaticAssertTypeEq<scaled3::scale_factor, three_point_five>();
  ::testing::StaticAssertTypeEq<scaled6::scale_factor, three_point_five>();
}

/// @test Can cancel inverse class scales with duplicates
TEST(ScaledUnits, CanCancelInverseClassScalesWithDuplicates) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  using scaled = arene::base::scaled_unit<my_unit, three_point_five>;
  using scaled2 =
      arene::base::scaled_unit<arene::base::scaled_unit<arene::base::scaled_unit<scaled, six>, six>, three_point_five>;
  using scaled3 = arene::base::scaled_unit<scaled2, arene::base::inverse_unit_scale_t<six>>;
  using scaled4 = arene::base::scaled_unit<scaled3, arene::base::inverse_unit_scale_t<six>>;
  using scaled5 = arene::base::scaled_unit<scaled4, arene::base::inverse_unit_scale_t<three_point_five>>;

  STATIC_ASSERT_NEAR(
      scaled3::scale_factor::value,
      three_point_five::value * three_point_five::value * six::value,
      0.0000001
  );
  ::testing::StaticAssertTypeEq<scaled5::scale_factor, three_point_five>();
}

/// @test Order doesn't matter when combining scales
TEST(ScaledUnits, OrderDoesNotMatterWhenCombining) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  ::testing::StaticAssertTypeEq<
      arene::base::
          scaled_unit<arene::base::scaled_unit<arene::base::scaled_unit<my_unit, std::kilo>, three_point_five>, six>,
      arene::base::
          scaled_unit<arene::base::scaled_unit<arene::base::scaled_unit<my_unit, six>, std::kilo>, three_point_five>>();
  ::testing::StaticAssertTypeEq<
      arene::base::
          scaled_unit<arene::base::scaled_unit<arene::base::scaled_unit<my_unit, three_point_five>, std::kilo>, six>,
      arene::base::
          scaled_unit<arene::base::scaled_unit<arene::base::scaled_unit<my_unit, six>, std::kilo>, three_point_five>>();
  ::testing::StaticAssertTypeEq<
      arene::base::
          scaled_unit<arene::base::scaled_unit<arene::base::scaled_unit<my_unit, three_point_five>, six>, std::kilo>,
      arene::base::
          scaled_unit<arene::base::scaled_unit<arene::base::scaled_unit<my_unit, six>, std::kilo>, three_point_five>>();
  ::testing::StaticAssertTypeEq<
      arene::base::
          scaled_unit<arene::base::scaled_unit<arene::base::scaled_unit<my_unit, six>, three_point_five>, std::kilo>,
      arene::base::
          scaled_unit<arene::base::scaled_unit<arene::base::scaled_unit<my_unit, six>, std::kilo>, three_point_five>>();
}

/// @test Combining scale values multiplies them. This test checks the internal implementation function
TEST(ScaledUnits, CombiningScales) {
  double first{2.5};
  double second{4.4};
  double third{10.0};

  ASSERT_NEAR(
      arene::base::scaled_unit_detail::combined_scale_value({first, second, third}),
      first * second * third,
      0.0000001
  );

  first = 4.4;
  second = 10;
  third = 2.5;

  ASSERT_NEAR(
      arene::base::scaled_unit_detail::combined_scale_value({first, second, third}),
      first * second * third,
      0.0000001
  );

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "We're checking that the unaltered values are correct");
  ASSERT_EQ(arene::base::scaled_unit_detail::scale_value<six>(), 6.);
  ASSERT_EQ(arene::base::scaled_unit_detail::scale_value<three_point_five>(), 3.5);
  ASSERT_EQ(arene::base::scaled_unit_detail::scale_value<std::kilo>(), 1000.);
  ARENE_IGNORE_END();
}

}  // namespace
