// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/units.hpp"
#include "arene/base/units/kind_with_exponent_fwd.hpp"

namespace {

/// @test A quantity kind can be raised to an exponent
TEST(QuantityKinds, AKindCanBeRaisedToAnExponent) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class other_kind : public arene::base::quantity_kind<other_kind> {};

  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<my_kind::with_exponent<3>>);
  STATIC_ASSERT_FALSE(std::is_same<my_kind, my_kind::with_exponent<3>>::value);
  STATIC_ASSERT_FALSE(std::is_same<my_kind::with_exponent<3>, my_kind::with_exponent<2>>::value);
  STATIC_ASSERT_FALSE(std::is_same<other_kind::with_exponent<3>, my_kind::with_exponent<3>>::value);
}

/// @test A derived kind raised to an exponent is derived from the parent kind raised to that exponent
TEST(QuantityKinds, DerivedKindRaisedToExponentIsDerivedFromParentKindRaisedToExponent) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, my_kind> {};

  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<derived_kind::with_exponent<3>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_base_quantity_kind_of_v<my_kind::with_exponent<3>, derived_kind::with_exponent<3>>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<my_kind, derived_kind::with_exponent<3>>);
}

/// @test A quantity kind raised to an exponent of one is that kind
TEST(QuantityKinds, AKindRaisedToAnExponentOfOneIsThatKind) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};

  STATIC_ASSERT_TRUE(std::is_same<my_kind, my_kind::with_exponent<1>>::value);
  STATIC_ASSERT_TRUE(std::is_same<my_kind, my_kind::with_exponent<-1>::with_exponent<-1>>::value);
}

/// @test A quantity unit raised to an exponent of one is that unit
TEST(QuantityKinds, AUnitRaisedToAnExponentOfOneIsThatUnit) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  STATIC_ASSERT_TRUE(std::is_same<my_unit, my_unit::with_exponent<1>>::value);
  STATIC_ASSERT_TRUE(std::is_same<my_unit, my_unit::with_exponent<-1>::with_exponent<-1>>::value);
}

/// @test Units raised to an exponent are still units
TEST(QuantityKinds, UnitsRaisedToAnExponentAreStillUnits) {
  class my_kind;  // IWYU pragma: keep
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  class my_kind : public arene::base::quantity_kind<my_kind, my_unit> {};

  using square_kind = my_kind::with_exponent<2>;
  using square_unit = my_unit::with_exponent<2>;

  STATIC_ASSERT_TRUE(arene::base::is_unit_v<square_unit>);
  STATIC_ASSERT_TRUE(std::is_same<square_unit::unit_kind_type, square_kind>::value);
}

/// @test A kind with units raised to an exponent has units that are the original units raised to that exponent
TEST(QuantityKinds, DefaultUnitsRaisedToExponentWithKind) {
  class my_kind;  // IWYU pragma: keep
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  class my_kind : public arene::base::quantity_kind<my_kind, my_unit> {};

  using square_kind = my_kind::with_exponent<2>;
  using square_unit = my_unit::with_exponent<2>;

  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<square_kind>);
  STATIC_ASSERT_TRUE(arene::base::is_unit_v<square_unit>);

  STATIC_ASSERT_TRUE(arene::base::has_selected_unit_v<square_kind>);
  STATIC_ASSERT_TRUE(std::is_same<arene::base::selected_unit_for_t<square_kind>, square_unit>::value);
}

/// @test Units raised to an exponent are for the base kind raised to the exponent
TEST(QuantityKinds, UnitsRaisedToAnExponentAreUnitsForBaseKindToExponent) {
  class my_kind;  // IWYU pragma: keep
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  class my_kind : public arene::base::quantity_kind<my_kind, my_unit> {};

  using square_kind = my_kind::with_exponent<2>;
  using square_unit = my_unit::with_exponent<2>;

  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<square_unit, square_kind>);
}

/// @test Units raised to an exponent are also units for the derived kind raised to the exponent
TEST(QuantityKinds, UnitsRaisedToAnExponentAreUnitsForDerivedKindToExponent) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, my_kind> {};

  using square_kind = derived_kind::with_exponent<2>;
  using square_unit = my_unit::with_exponent<2>;

  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<my_unit, derived_kind>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<square_unit, square_kind>);
}

/// @test A derived kind with default units raised to an exponent has default units that are the original default units
/// raised to the same exponent
TEST(QuantityKinds, DerivedKindWithUnitsRaisedToExponentHasUnits) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, my_kind, my_unit> {};

  using square_kind = derived_kind::with_exponent<2>;
  using square_unit = my_unit::with_exponent<2>;

  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<my_unit, derived_kind>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<square_unit, square_kind>);
  STATIC_ASSERT_TRUE(arene::base::has_selected_unit_v<square_kind>);
  STATIC_ASSERT_TRUE(std::is_same<arene::base::selected_unit_for_t<square_kind>, square_unit>::value);
}

/// @test A kind derived from a kind with default units raised to an exponent has default units that are the original
/// default units raised to the same exponent
TEST(QuantityKinds, DerivedKindWithParentWithDefaultUnitsRaisedToExponentHasUnits) {
  class my_kind;  // IWYU pragma: keep
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  class my_kind : public arene::base::quantity_kind<my_kind, my_unit> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, my_kind> {};

  using square_kind = derived_kind::with_exponent<2>;
  using square_unit = my_unit::with_exponent<2>;

  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<my_unit, derived_kind>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<square_unit, square_kind>);
  STATIC_ASSERT_TRUE(arene::base::has_selected_unit_v<square_kind>);
  STATIC_ASSERT_TRUE(std::is_same<arene::base::selected_unit_for_t<square_kind>, square_unit>::value);
}

/// @test A kind with default units derived from a kind with default units raised to an exponent has default units that
/// are the original default units of the derived kind raised to the same exponent
TEST(QuantityKinds, DerivedKindWithDefaultUnitsParentWithOtherDefaultUnitsRaisedToExponentHasUnits) {
  class my_kind;  // IWYU pragma: keep
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  class other_unit : public arene::base::quantity_unit<other_unit, my_kind> {};
  class my_kind : public arene::base::quantity_kind<my_kind, my_unit> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, my_kind, other_unit> {};

  using square_kind = derived_kind::with_exponent<2>;
  using square_unit = other_unit::with_exponent<2>;

  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<square_unit, square_kind>);
  STATIC_ASSERT_TRUE(arene::base::has_selected_unit_v<square_kind>);
  STATIC_ASSERT_TRUE(std::is_same<arene::base::selected_unit_for_t<square_kind>, square_unit>::value);
}

/// @test A quantity kind raised to an exponent and then raised again multiplies the exponents
TEST(QuantityKinds, ExponentsStack) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};

  STATIC_ASSERT_TRUE(std::is_same<my_kind::with_exponent<6>, my_kind::with_exponent<2>::with_exponent<3>>::value);
  STATIC_ASSERT_TRUE(std::is_same<my_kind::with_exponent<-6>, my_kind::with_exponent<2>::with_exponent<-3>>::value);
  STATIC_ASSERT_TRUE(std::is_same<my_kind::with_exponent<-6>, my_kind::with_exponent<-2>::with_exponent<3>>::value);
}

/// @test A quantity kind raised to an exponent and then raised again multiplies the exponents
TEST(QuantityKinds, ExponentsStackForUnits) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  STATIC_ASSERT_TRUE(std::is_same<my_unit::with_exponent<6>, my_unit::with_exponent<2>::with_exponent<3>>::value);
  STATIC_ASSERT_TRUE(std::is_same<my_unit::with_exponent<-6>, my_unit::with_exponent<2>::with_exponent<-3>>::value);
  STATIC_ASSERT_TRUE(std::is_same<my_unit::with_exponent<-6>, my_unit::with_exponent<-2>::with_exponent<3>>::value);
}

/// @test A quantity kind can be raised to an exponent via alias template
TEST(QuantityKinds, AKindCanBeRaisedToAnExponentByTypeAlias) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class other_kind : public arene::base::quantity_kind<other_kind> {};

  STATIC_ASSERT_TRUE(std::is_same<arene::base::kind_with_exponent_t<my_kind, 3>, my_kind::with_exponent<3>>::value);
  STATIC_ASSERT_TRUE(std::is_same<arene::base::kind_with_exponent_t<my_kind, 5>, my_kind::with_exponent<5>>::value);
  STATIC_ASSERT_TRUE(std::is_same<arene::base::kind_with_exponent_t<other_kind, 2>, other_kind::with_exponent<2>>::value
  );
}

/// @test A quantity kind can be raised to an exponent of zero
TEST(QuantityKinds, AKindCanBeRaisedToAnExponentOfZero) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class other_kind : public arene::base::quantity_kind<other_kind> {};

  STATIC_ASSERT_TRUE(std::is_same<arene::base::kind_with_exponent_t<my_kind, 0>, my_kind::with_exponent<0>>::value);
  STATIC_ASSERT_TRUE(std::is_same<my_kind::with_exponent<5>::with_exponent<0>, my_kind::with_exponent<0>>::value);
  STATIC_ASSERT_FALSE(std::is_same<other_kind::with_exponent<0>, my_kind::with_exponent<0>>::value);
}

}  // namespace
