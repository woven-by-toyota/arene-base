// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/ratio.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/units.hpp"
#include "arene/base/units/kind_with_exponent_fwd.hpp"

namespace {
/// @test "Combining" a single kind just yields that kind
TEST(CombiningQuantityKinds, CombiningASingleKindYieldsThatKind) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class second_kind : public arene::base::quantity_kind<second_kind> {};

  ::testing::StaticAssertTypeEq<first_kind, arene::base::combine_kinds_t<first_kind>>();
  ::testing::StaticAssertTypeEq<second_kind, arene::base::combine_kinds_t<second_kind>>();
}

/// @test Combining two kinds yields a quantity kind that is neither, but the same both ways round
TEST(CombiningQuantityKinds, CombiningTwoKindsYieldsAUniqueKind) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class second_kind : public arene::base::quantity_kind<second_kind> {};

  STATIC_ASSERT_FALSE(std::is_same<first_kind, arene::base::combine_kinds_t<first_kind, second_kind>>::value);
  STATIC_ASSERT_FALSE(std::is_same<second_kind, arene::base::combine_kinds_t<first_kind, second_kind>>::value);
  STATIC_ASSERT_TRUE(std::is_same<
                     arene::base::combine_kinds_t<second_kind, first_kind>,
                     arene::base::combine_kinds_t<first_kind, second_kind>>::value);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<arene::base::combine_kinds_t<first_kind, second_kind>>);
  STATIC_ASSERT_FALSE(arene::base::is_unit_v<arene::base::combine_kinds_t<first_kind, second_kind>>);
}

/// @test Combining two of the same kind yields that kind squared
TEST(CombiningQuantityKinds, CombiningTwoInstancesOfTheSameKindYieldsASquareKind) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class second_kind : public arene::base::quantity_kind<second_kind> {};

  STATIC_ASSERT_TRUE(
      std::is_same<first_kind::with_exponent<2>, arene::base::combine_kinds_t<first_kind, first_kind>>::value
  );
  STATIC_ASSERT_TRUE(
      std::is_same<second_kind::with_exponent<2>, arene::base::combine_kinds_t<second_kind, second_kind>>::value
  );
}

/// @test Combining a kind with an exponent with an instance of the same base kind yields that kind with an increased
/// exponent
TEST(CombiningQuantityKinds, CombiningInstancesWithExponentAndInstanceOfTheSameKindYieldsKindWithIncreasedExponent) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};

  STATIC_ASSERT_TRUE(std::is_same<
                     first_kind::with_exponent<3>,
                     arene::base::combine_kinds_t<first_kind::with_exponent<2>, first_kind>>::value);
  STATIC_ASSERT_TRUE(std::is_same<
                     first_kind::with_exponent<3>,
                     arene::base::combine_kinds_t<first_kind, first_kind::with_exponent<2>>>::value);
  STATIC_ASSERT_TRUE(
      std::is_same<first_kind::with_exponent<3>, arene::base::combine_kinds_t<first_kind, first_kind, first_kind>>::
          value
  );
}

/// @test Combining a kind with an exponent with another instance of the same base kind with a different expoenent
/// yields that kind raised to the sum of the exponents
TEST(CombiningQuantityKinds, CombiningMultipleInstancesOfKindWithExponentCombinesExponents) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};

  STATIC_ASSERT_TRUE(std::is_same<
                     first_kind::with_exponent<5>,
                     arene::base::combine_kinds_t<first_kind::with_exponent<2>, first_kind::with_exponent<3>>>::value);
  STATIC_ASSERT_TRUE(std::is_same<
                     first_kind::with_exponent<4>,
                     arene::base::combine_kinds_t<first_kind::with_exponent<2>, first_kind::with_exponent<2>>>::value);
}

/// @test Combining a mix of two kinds with and without exponent groups kinds
TEST(CombiningQuantityKinds, CombiningAMixOfTwoKindsWithAndWithoutExponentsGroupsKinds) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class second_kind : public arene::base::quantity_kind<second_kind> {};

  STATIC_ASSERT_TRUE(std::is_same<
                     arene::base::combine_kinds_t<first_kind::with_exponent<5>, second_kind::with_exponent<3>>,
                     arene::base::combine_kinds_t<
                         first_kind,
                         second_kind::with_exponent<2>,
                         first_kind::with_exponent<2>,
                         second_kind,
                         first_kind::with_exponent<2>>>::value);
}

/// @test Positive and negative exponents do not cancel
TEST(CombiningQuantityKinds, PositiveAndNegativeExponentsDoNotCancel) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};

  STATIC_ASSERT_FALSE(std::is_same<
                      arene::base::combine_kinds_t<first_kind::with_exponent<2>, first_kind::with_exponent<-1>>,
                      first_kind>::value);
  STATIC_ASSERT_FALSE(std::is_same<
                      arene::base::combine_kinds_t<first_kind::with_exponent<3>, first_kind::with_exponent<-1>>,
                      first_kind::with_exponent<2>>::value);
  STATIC_ASSERT_TRUE(
      std::is_same<
          arene::base::combine_kinds_t<first_kind::with_exponent<4>, first_kind::with_exponent<-2>>,
          arene::base::combine_kinds_t<first_kind::with_exponent<2>, first_kind::with_exponent<-1>>::with_exponent<2>>::
          value
  );
}

/// @test Combining the results of previously combining units is the same as if everything was combined together
TEST(CombiningQuantityKinds, CombiningAlreadyCombinedTypesIsTheSameAsCombiningAllAtOnce) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class second_kind : public arene::base::quantity_kind<second_kind> {};

  using combination_a = arene::base::combine_kinds_t<first_kind::with_exponent<5>, second_kind::with_exponent<3>>;
  using combination_b = arene::base::
      combine_kinds_t<first_kind::with_exponent<2>, second_kind::with_exponent<-3>, second_kind::with_exponent<1>>;

  ::testing::StaticAssertTypeEq<
      arene::base::
          combine_kinds_t<first_kind::with_exponent<7>, second_kind::with_exponent<4>, second_kind::with_exponent<-3>>,
      arene::base::combine_kinds_t<combination_a, combination_b>>();
}

/// @test Raising a combination to an exponent is the same as combining the elements raised to that exponent
TEST(CombiningQuantityKinds, RaisingCombinationToExponentIsCombinedElementsRaisedToExponent) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class second_kind : public arene::base::quantity_kind<second_kind> {};
  class third_kind : public arene::base::quantity_kind<third_kind> {};

  ::testing::StaticAssertTypeEq<
      arene::base::combine_kinds_t<first_kind::with_exponent<7>, second_kind, third_kind::with_exponent<-3>>::
          with_exponent<2>,
      arene::base::
          combine_kinds_t<first_kind::with_exponent<14>, second_kind::with_exponent<2>, third_kind::with_exponent<-6>>>(
  );
}

/// @test Combining two units yields a quantity unit that is a unit for the corresponding combined base kinds
TEST(CombiningQuantityKinds, CombiningTwoUnitsYieldsAUnit) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class first_unit : public arene::base::quantity_unit<first_unit, first_kind> {};
  class second_kind : public arene::base::quantity_kind<second_kind> {};
  class second_unit : public arene::base::quantity_unit<second_unit, second_kind> {};

  using combined_unit = arene::base::combine_kinds_t<first_unit, second_unit>;
  using combined_kind = arene::base::combine_kinds_t<first_kind, second_kind>;

  STATIC_ASSERT_TRUE(arene::base::is_unit_v<combined_unit>);
  STATIC_ASSERT_FALSE(arene::base::is_unit_v<combined_kind>);

  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<combined_unit, combined_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_possible_unit_for_v<combined_unit, first_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_possible_unit_for_v<combined_unit, second_kind>);
}

/// @test Combining a unit and a non unit is not a unit
TEST(CombiningQuantityKinds, CombiningUnitAndNonUnitIsNotAUnit) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class first_unit : public arene::base::quantity_unit<first_unit, first_kind> {};

  using combined_kind = arene::base::combine_kinds_t<first_kind, first_unit>;

  STATIC_ASSERT_FALSE(arene::base::is_unit_v<combined_kind>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<combined_kind>);
}

/// @test Combining two derived kinds yields a quantity kind that is derived from the combination of base kinds
TEST(CombiningQuantityKinds, CombinedDerivedKindsIsDerivedFromCombinedBaseKinds) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class second_kind : public arene::base::quantity_kind<second_kind> {};
  class first_derived_kind : public arene::base::quantity_kind<first_derived_kind, first_kind> {};
  class second_derived_kind : public arene::base::quantity_kind<second_derived_kind, second_kind> {};

  using combined_derived = arene::base::combine_kinds_t<first_derived_kind, second_derived_kind>;

  using first_derived_second_base = arene::base::combine_kinds_t<first_derived_kind, second_kind>;
  using second_derived_first_base = arene::base::combine_kinds_t<first_kind, second_derived_kind>;
  using both_bases = arene::base::combine_kinds_t<first_kind, second_kind>;

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<both_bases, combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<both_bases, first_derived_second_base>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<both_bases, second_derived_first_base>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<first_derived_second_base, combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<second_derived_first_base, combined_derived>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<first_derived_second_base, both_bases>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<second_derived_first_base, both_bases>);
}

/// @test Combining two derived kinds yields a quantity kind that is derived from the combination of base kinds with
/// different names. This duplicates the previous test for the case that the types order differently when combined
/// (since the ordering is based on the type name), and so different branches are evaluated.
TEST(CombiningQuantityKinds, CombinedDerivedKindsIsDerivedFromCombinedBaseKindsWithDifferentNames) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class second_kind : public arene::base::quantity_kind<second_kind> {};
  class first_derived_kind : public arene::base::quantity_kind<first_derived_kind, first_kind> {};
  class derived_second_kind : public arene::base::quantity_kind<derived_second_kind, second_kind> {};

  using combined_derived = arene::base::combine_kinds_t<first_derived_kind, derived_second_kind>;

  using first_derived_second_base = arene::base::combine_kinds_t<first_derived_kind, second_kind>;
  using derived_second_first_base = arene::base::combine_kinds_t<first_kind, derived_second_kind>;
  using both_bases = arene::base::combine_kinds_t<first_kind, second_kind>;

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<both_bases, combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<both_bases, first_derived_second_base>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<both_bases, derived_second_first_base>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<first_derived_second_base, combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<derived_second_first_base, combined_derived>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<first_derived_second_base, both_bases>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<derived_second_first_base, both_bases>);
}

/// @test Combining two derived kinds derived from the same base yields a quantity kind that is derived from the
/// combination of base kinds
TEST(CombiningQuantityKinds, CombinedDerivedKindsIsDerivedFromCombinedBaseKindsWithSameBase) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class first_derived_kind : public arene::base::quantity_kind<first_derived_kind, first_kind> {};
  class derived_second_kind : public arene::base::quantity_kind<derived_second_kind, first_kind> {};

  using combined_derived = arene::base::combine_kinds_t<first_derived_kind, derived_second_kind>;

  using first_derived_second_base = arene::base::combine_kinds_t<first_derived_kind, first_kind>;
  using derived_second_first_base = arene::base::combine_kinds_t<first_kind, derived_second_kind>;
  using both_bases = arene::base::combine_kinds_t<first_kind, first_kind>;
  using both_first_derived = arene::base::combine_kinds_t<first_derived_kind, first_derived_kind>;
  using both_second_derived = arene::base::combine_kinds_t<derived_second_kind, derived_second_kind>;

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<both_bases, both_first_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<both_bases, both_second_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<both_bases, combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<both_bases, first_derived_second_base>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<both_bases, derived_second_first_base>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<first_derived_second_base, combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<derived_second_first_base, combined_derived>);
}

/// @test A derived kind with an exponent yields a quantity kind that is derived from the combination of two kinds that
/// are both parents of the derived kind
TEST(CombiningQuantityKinds, DerivedKindWithExponentIsDerivedFromBases) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class intermediate_base : public arene::base::quantity_kind<intermediate_base, first_kind> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, intermediate_base> {};

  using combined_derived = derived_kind::with_exponent<2>;

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<
                     arene::base::combine_kinds_t<first_kind, intermediate_base>,
                     combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<
                     arene::base::combine_kinds_t<first_kind, derived_kind>,
                     combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<
                     arene::base::combine_kinds_t<derived_kind, intermediate_base>,
                     combined_derived>);
}

/// @test Combining three derived kinds yields a quantity kind that is derived from the combination of base kinds
TEST(CombiningQuantityKinds, CombinedDerivedKindsIsDerivedFromCombinedBaseKindsForThreeKinds) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class second_kind : public arene::base::quantity_kind<second_kind> {};
  class first_derived_kind : public arene::base::quantity_kind<first_derived_kind, first_kind> {};
  class derived_second_kind : public arene::base::quantity_kind<derived_second_kind, second_kind> {};
  class third_derived_kind : public arene::base::quantity_kind<third_derived_kind, second_kind> {};

  using combined_derived = arene::base::combine_kinds_t<first_derived_kind, derived_second_kind, third_derived_kind>;

  using first_derived_second_base_second_base =
      arene::base::combine_kinds_t<first_derived_kind, second_kind, second_kind>;
  using derived_second_first_base_second_base =
      arene::base::combine_kinds_t<first_kind, derived_second_kind, second_kind>;
  using all_bases = arene::base::combine_kinds_t<first_kind, second_kind, second_kind>;

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<all_bases, combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<all_bases, first_derived_second_base_second_base>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<all_bases, derived_second_first_base_second_base>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<first_derived_second_base_second_base, combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<derived_second_first_base_second_base, combined_derived>);
}

/// @test Combining two derived kinds with exponents derived from the same base yields a quantity kind that is derived
/// from the combination of base kinds
TEST(CombiningQuantityKinds, DerivedKindWithExponentIsDerivedFromBasesWithExponents) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class intermediate_base : public arene::base::quantity_kind<intermediate_base, first_kind> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, intermediate_base> {};

  using combined_derived = arene::base::combine_kinds_t<derived_kind::with_exponent<2>, intermediate_base>;

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<
                     arene::base::combine_kinds_t<first_kind::with_exponent<2>, intermediate_base>,
                     combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<first_kind::with_exponent<3>, combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<
                     arene::base::combine_kinds_t<first_kind, intermediate_base::with_exponent<2>>,
                     combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<
                     arene::base::combine_kinds_t<first_kind, intermediate_base::with_exponent<2>>,
                     derived_kind::with_exponent<3>>);
}

/// @test Combining two derived kinds with negative exponents derived from the same base yields a quantity kind that is
/// derived from the combination of base kinds
TEST(CombiningQuantityKinds, DerivedKindWithNegativeExponentIsDerivedFromBasesWithNegativeExponents) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class intermediate_base : public arene::base::quantity_kind<intermediate_base, first_kind> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, intermediate_base> {};

  using combined_derived =
      arene::base::combine_kinds_t<derived_kind::with_exponent<-2>, intermediate_base::with_exponent<-1>>;

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<
                     arene::base::combine_kinds_t<first_kind::with_exponent<-2>, intermediate_base::with_exponent<-1>>,
                     combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<first_kind::with_exponent<-3>, combined_derived>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<first_kind::with_exponent<3>, combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<
                     arene::base::combine_kinds_t<first_kind::with_exponent<-1>, intermediate_base::with_exponent<-2>>,
                     combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<
                     arene::base::combine_kinds_t<first_kind::with_exponent<-1>, intermediate_base::with_exponent<-2>>,
                     derived_kind::with_exponent<-3>>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<
                      arene::base::combine_kinds_t<first_kind::with_exponent<1>, intermediate_base::with_exponent<2>>,
                      derived_kind::with_exponent<-3>>);
}

/// @test A base quantity kind with equal positive and negative or zero exponent is not a base for a combination of a
/// derived kind with equal positive and negative exponents unless they match exactly
TEST(CombiningQuantityKinds, ZeroExponentIsNotTheSameAsPlusMinusIdenticalExponents) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, first_kind> {};

  STATIC_ASSERT_TRUE(arene::base::
                         is_base_quantity_kind_of_v<first_kind::with_exponent<0>, derived_kind::with_exponent<0>>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<
                      first_kind::with_exponent<0>,
                      arene::base::combine_kinds_t<derived_kind::with_exponent<1>, derived_kind::with_exponent<-1>>>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<
                      first_kind::with_exponent<0>,
                      arene::base::combine_kinds_t<derived_kind::with_exponent<-2>, derived_kind::with_exponent<2>>>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<
                      arene::base::combine_kinds_t<first_kind::with_exponent<1>, first_kind::with_exponent<-1>>,
                      arene::base::combine_kinds_t<derived_kind::with_exponent<-2>, derived_kind::with_exponent<2>>>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<
                     arene::base::combine_kinds_t<first_kind::with_exponent<2>, first_kind::with_exponent<-2>>,
                     arene::base::combine_kinds_t<derived_kind::with_exponent<-2>, derived_kind::with_exponent<2>>>);
}

/// @test Combining two derived kinds with mixed exponents derived from the same base yields a quantity kind that is
/// derived from the combination of base kinds
TEST(CombiningQuantityKinds, DerivedKindWithMixedExponentIsDerivedFromBasesWithMixedExponents) {
  class first_kind : public arene::base::quantity_kind<first_kind> {};
  class intermediate_base : public arene::base::quantity_kind<intermediate_base, first_kind> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, intermediate_base> {};

  using combined_derived = arene::base::combine_kinds_t<
      derived_kind::with_exponent<-5>,
      derived_kind::with_exponent<3>,
      intermediate_base::with_exponent<-10>,
      intermediate_base::with_exponent<9>>;

  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<
                      arene::base::combine_kinds_t<first_kind::with_exponent<-2>, intermediate_base::with_exponent<-1>>,
                      combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<
                     arene::base::combine_kinds_t<
                         first_kind::with_exponent<-5>,
                         first_kind::with_exponent<3>,
                         intermediate_base::with_exponent<-10>,
                         intermediate_base::with_exponent<9>>,
                     combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<
                     arene::base::combine_kinds_t<
                         first_kind::with_exponent<-8>,
                         first_kind::with_exponent<10>,
                         intermediate_base::with_exponent<-7>,
                         intermediate_base::with_exponent<2>>,
                     combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<
                     arene::base::combine_kinds_t<
                         first_kind::with_exponent<-8>,
                         first_kind::with_exponent<1>,
                         intermediate_base::with_exponent<-7>,
                         intermediate_base::with_exponent<11>>,
                     combined_derived>);
}

/// @test combining scaled units does not decay the scaled unit
TEST(CombiningQuantityKinds, WithScaledUnits) {
  class length : public arene::base::quantity_kind<length> {};
  class length_unit : public arene::base::quantity_unit<length_unit, length> {};
  using scaled = arene::base::scaled_unit<length_unit, std::kilo>;

  using arene::base::combine_kinds_t;

  STATIC_ASSERT_TRUE(  //
      std::is_same<    //
          scaled,
          combine_kinds_t<scaled>  //
          >::value
  );

  STATIC_ASSERT_TRUE(  //
      std::is_same<    //
          scaled::with_exponent<2>,
          combine_kinds_t<scaled, scaled>  //
          >::value
  );

  STATIC_ASSERT_FALSE(                           //
      std::is_same<                              //
          combine_kinds_t<length_unit, scaled>,  //
          combine_kinds_t<scaled, scaled>        //
          >::value
  );
}

/// @test A combined kind raised to the first power is the same combined kind
TEST(CombiningQuantityKinds, CombinedKindRaisedToExponent1) {
  class length : public arene::base::quantity_kind<length> {};
  class time : public arene::base::quantity_kind<time> {};

  using combined_kind = arene::base::combine_kinds_t<  //
      length,
      arene::base::kind_with_exponent_t<time, -1>,
      arene::base::kind_with_exponent_t<time, -1>>;

  STATIC_ASSERT_TRUE(  //
      std::is_same<    //
          combined_kind,
          arene::base::kind_with_exponent_t<combined_kind, 1>>::value
  );
}

/// @test ensure that the sink kind set has the same size as the source kind set
/// in order to be a base quantity
TEST(CombiningQuantityKinds, NumberOfSinksMustEqualSources) {
  class length : public arene::base::quantity_kind<length> {};
  class width : public arene::base::quantity_kind<width, length> {};

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<       //
                     arene::base::combine_kinds_t<length, length>,  //
                     arene::base::combine_kinds_t<width, length>    //
                     >);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<               //
                      arene::base::combine_kinds_t<length, length, length>,  //
                      arene::base::combine_kinds_t<width, length>            //
                      >);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<             //
                      arene::base::combine_kinds_t<length, length>,        //
                      arene::base::combine_kinds_t<width, length, length>  //
                      >);
}

/// @test non-positive exponents are handled correctly when determining if
/// combined kinds are base quantities of each other
TEST(CombiningQuantityKinds, NonPositiveExponentValues) {
  class length : public arene::base::quantity_kind<length> {};
  class time : public arene::base::quantity_kind<time> {};

  class width : public arene::base::quantity_kind<width, length> {};

  class meters : public arene::base::quantity_unit<meters, length> {};
  class hours : public arene::base::quantity_unit<hours, time> {};

  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<                         //
                      arene::base::combine_kinds_t<meters, hours::with_exponent<-1>>,  //
                      arene::base::combine_kinds_t<length, time::with_exponent<-1>>    //
                      >);

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<                        //
                     arene::base::combine_kinds_t<length, time::with_exponent<-1>>,  //
                     arene::base::combine_kinds_t<length, hours::with_exponent<-1>>  //
                     >);

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<                        //
                     arene::base::combine_kinds_t<length, time::with_exponent<-1>>,  //
                     arene::base::combine_kinds_t<width, hours::with_exponent<-1>>   //
                     >);
}

/// @test all source and sink quanity kinds must have matches (i.e. a bijection)
TEST(CombiningQuantityKinds, AllKindsMustMatch) {
  class length : public arene::base::quantity_kind<length> {};
  class width : public arene::base::quantity_kind<width, length> {};
  class time : public arene::base::quantity_kind<time> {};

  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<             //
                      arene::base::combine_kinds_t<length, length, time>,  //
                      arene::base::combine_kinds_t<width, length, length>  // derived kind has no "time"
                      >);
}

/// @test A quantity unit derived from a combination of quantity units is a possible unit for a quantity kind derived
/// from the base quantity kinds of those units
TEST(CombiningQuantityKinds, UnitsDerivedFromCombinedUnitsArePossibleUnitsForCombinedBaseKinds) {
  class base1 : public arene::base::quantity_kind<base1> {};
  class base2 : public arene::base::quantity_kind<base2> {};

  class unit1 : public arene::base::quantity_unit<unit1, base1> {};
  class unit2 : public arene::base::quantity_unit<unit2, base2> {};

  using combined_units = arene::base::combine_kinds_t<unit1, unit2>;
  using combined_bases = arene::base::combine_kinds_t<base1, base2>;

  class derived_unit : public arene::base::quantity_unit<derived_unit, combined_units> {};

  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<combined_units, combined_bases>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<derived_unit, combined_bases>);
}

/// @test A quantity unit derived from a combination of quantity units with exponents is a possible unit for a quantity
/// kind derived from the base quantity kinds of those units
TEST(CombiningQuantityKinds, UnitsDerivedFromCombinedUnitsWithExponentArePossibleUnitsForCombinedBaseKinds) {
  class base1 : public arene::base::quantity_kind<base1> {};
  class base2 : public arene::base::quantity_kind<base2> {};

  class unit1 : public arene::base::quantity_unit<unit1, base1> {};
  class unit2 : public arene::base::quantity_unit<unit2, base2> {};

  using combined_units = arene::base::combine_kinds_t<unit1, unit2::with_exponent<2>>;
  using combined_bases = arene::base::combine_kinds_t<base1, base2::with_exponent<2>>;

  class derived_unit : public arene::base::quantity_unit<derived_unit, combined_units> {};

  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<combined_units, combined_bases>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<derived_unit, combined_bases>);
}

/// @test A combination of a base quantity unit and a quantity unit derived from a combination of quantity units with
/// exponents is a possible unit for a quantity kind derived from the base quantity kinds of those units
TEST(CombiningQuantityKinds, CombinedUnitsDerivedFromCombinedUnitsWithExponentArePossibleUnitsForCombinedBaseKinds) {
  class base1 : public arene::base::quantity_kind<base1> {};
  class base2 : public arene::base::quantity_kind<base2> {};

  class unit1 : public arene::base::quantity_unit<unit1, base1> {};
  class unit2 : public arene::base::quantity_unit<unit2, base2> {};

  using combined_units = arene::base::combine_kinds_t<unit1, unit2::with_exponent<2>>;
  using combined_bases = arene::base::combine_kinds_t<base1, base2::with_exponent<2>>;

  class derived_unit : public arene::base::quantity_unit<derived_unit, combined_units> {};

  using combined_derived = arene::base::combine_kinds_t<derived_unit, unit1::with_exponent<-1>>;
  using extended_combined_bases = arene::base::combine_kinds_t<combined_bases, base1::with_exponent<-1>>;

  STATIC_ASSERT_TRUE(arene::base::is_unit_v<combined_derived>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<combined_derived, extended_combined_bases>);
}

/// @test A combination of kinds is not a base for another combination if there is any kinds in the base for which there
/// are no matching kinds in the other combination
TEST(CombiningQuantityKinds, AllUnitsMustMatch) {
  class length : public arene::base::quantity_kind<length> {};
  class width : public arene::base::quantity_kind<width, length> {};
  class time : public arene::base::quantity_kind<time> {};

  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<             //
                      arene::base::combine_kinds_t<length, length, time>,  //
                      arene::base::combine_kinds_t<width, length, length>  //
                      >);
}

}  // namespace
