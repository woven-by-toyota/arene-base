// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/units.hpp"

namespace {

/// @test Ensure that types not explicitly declared as such are not quantity kinds
TEST(QuantityKinds, DetectNonQuantityKinds) {
  class local {};

  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<local>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<int>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<double>);
}

/// @test Ensure that a kind can be declared
TEST(QuantityKinds, CanDeclareCustomKinds) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};

  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<my_kind>);
}

/// @test Ensure that a unit can be declared
TEST(QuantityKinds, CanDeclareCustomUnits) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<my_unit>);
}

/// @test Ensure @c is_quantity_kind_v ignores CV qualifiers
TEST(QuantityKinds, IsQuantityKindIgnoresCVQualifiers) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class other_kind : public arene::base::quantity_kind<other_kind> {};
  class local {};

  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<my_kind>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<my_kind const>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<my_kind volatile>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<my_kind const volatile>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<other_kind>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<other_kind const>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<other_kind volatile>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<other_kind const volatile>);

  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<local>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<int>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<double>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<local const>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<int const>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<double const>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<local volatile>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<int volatile>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<double volatile>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<local const volatile>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<int const volatile>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<double const volatile>);
}

/// @test Ensure non-unit types are not units for kinds
TEST(QuantityKinds, KindsAndBasicTypesAreNotUnitsForKinds) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class other_kind : public arene::base::quantity_kind<other_kind> {};

  STATIC_ASSERT_FALSE(arene::base::is_possible_unit_for_v<other_kind, my_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_possible_unit_for_v<my_kind, my_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_possible_unit_for_v<int, my_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_possible_unit_for_v<my_kind, int>);
}

/// @test Ensure that a declared unit is only a unit for the specified kind
TEST(QuantityKinds, CustomUnitsAreOnlyUnitsForSpecifiedKinds) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  class other_kind : public arene::base::quantity_kind<other_kind> {};
  class other_unit : public arene::base::quantity_unit<other_unit, other_kind> {};

  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<my_unit, my_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_possible_unit_for_v<my_unit, other_kind>);

  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<other_unit, other_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_possible_unit_for_v<other_unit, my_kind>);
}

/// @test Basic types and declared kinds are not base kinds of each other
TEST(QuantityKinds, BasicTypesAndDeclaredKindsNotBaseKindsOfEachOther) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class other_kind : public arene::base::quantity_kind<other_kind> {};
  class non_kind {};

  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<int, int>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<int, double>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<double, int>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<int, my_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<my_kind, int>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<double, my_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<my_kind, double>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<non_kind, my_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<my_kind, non_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<int, non_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<non_kind, int>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<other_kind, my_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<my_kind, other_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<my_kind, my_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<other_kind, other_kind>);
}

/// @test A kind is a base kind for its derived kinds
TEST(QuantityKinds, AKindIsABaseKindForItsDerivedKinds) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, my_kind> {};
  class third_kind : public arene::base::quantity_kind<third_kind, derived_kind> {};

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<my_kind, derived_kind>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<my_kind, third_kind>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<derived_kind, third_kind>);

  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<derived_kind, my_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<third_kind, my_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<third_kind, derived_kind>);
}

/// @test A kind is not a base kind for sibling kinds
TEST(QuantityKinds, AKindIsNotABaseKindForSiblingKinds) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, my_kind> {};
  class other_derived_kind : public arene::base::quantity_kind<other_derived_kind, my_kind> {};

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<my_kind, derived_kind>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<my_kind, other_derived_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<derived_kind, other_derived_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<other_derived_kind, derived_kind>);
}

/// @test A kind is a base kind for its units
TEST(QuantityKinds, AKindIsABaseKindForItsUnits) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<my_kind, my_unit>);
  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<my_unit, my_kind>);
}

/// @test A kind is not a base kind for other units
TEST(QuantityKinds, AKindIsNotABaseKindForOtherUnits) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class other_kind : public arene::base::quantity_kind<other_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, other_kind> {};

  STATIC_ASSERT_FALSE(arene::base::is_base_quantity_kind_of_v<my_kind, my_unit>);
}

/// @test The "units for" are not defined for a kind without a unit
TEST(QuantityKinds, UnitsForNotDefinedForPlainKinds) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class other_kind : public arene::base::quantity_kind<other_kind> {};

  STATIC_ASSERT_FALSE(arene::base::has_selected_unit_v<my_kind>);
  STATIC_ASSERT_FALSE(arene::base::has_selected_unit_v<other_kind>);
}

/// @test The "units for" a unit are that unit
TEST(QuantityKinds, UnitsForUnitIsIdentity) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  class other_unit : public arene::base::quantity_unit<other_unit, my_kind> {};

  STATIC_ASSERT_TRUE(arene::base::has_selected_unit_v<my_unit>);
  STATIC_ASSERT_TRUE(arene::base::has_selected_unit_v<other_unit>);
  STATIC_ASSERT_TRUE(std::is_same<arene::base::selected_unit_for_t<my_unit>, my_unit>::value);
  STATIC_ASSERT_TRUE(std::is_same<arene::base::selected_unit_for_t<other_unit>, other_unit>::value);
}

/// @test The "units for" a kind with a default unit are that unit
TEST(QuantityKinds, UnitsForKindWithDefaultUnit) {
  class my_kind_with_default_unit;  // IWYU pragma: keep
  class default_unit : public arene::base::quantity_unit<default_unit, my_kind_with_default_unit> {};
  class my_kind_with_default_unit : public arene::base::quantity_kind<my_kind_with_default_unit, default_unit> {};

  STATIC_ASSERT_TRUE(arene::base::is_unit_v<default_unit>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<my_kind_with_default_unit>);
  STATIC_ASSERT_TRUE(arene::base::has_selected_unit_v<my_kind_with_default_unit>);
  STATIC_ASSERT_TRUE(std::is_same<arene::base::selected_unit_for_t<my_kind_with_default_unit>, default_unit>::value);
}

/// @test The "units for" a kind with a default unit and parent are that unit
TEST(QuantityKinds, UnitsForKindWithDefaultUnitAndParent) {
  class base_kind : public arene::base::quantity_kind<base_kind> {};
  class my_kind_with_default_unit;  // IWYU pragma: keep
  class default_unit : public arene::base::quantity_unit<default_unit, my_kind_with_default_unit> {};
  class my_kind_with_default_unit
      : public arene::base::quantity_kind<my_kind_with_default_unit, base_kind, default_unit> {};

  STATIC_ASSERT_TRUE(arene::base::is_unit_v<default_unit>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<my_kind_with_default_unit>);
  STATIC_ASSERT_TRUE(arene::base::has_selected_unit_v<my_kind_with_default_unit>);
  STATIC_ASSERT_TRUE(std::is_same<arene::base::selected_unit_for_t<my_kind_with_default_unit>, default_unit>::value);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<base_kind, my_kind_with_default_unit>);
}

/// @test The "units for" a derived kind from a parent with a default unit are that unit
TEST(QuantityKinds, UnitsForDerivedKindWithParentHavingDefaultUnit) {
  class my_kind_with_default_unit;  // IWYU pragma: keep
  class default_unit : public arene::base::quantity_unit<default_unit, my_kind_with_default_unit> {};
  class my_kind_with_default_unit : public arene::base::quantity_kind<my_kind_with_default_unit, default_unit> {};

  class derived_kind : public arene::base::quantity_kind<derived_kind, my_kind_with_default_unit> {};

  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<derived_kind>);
  STATIC_ASSERT_TRUE(arene::base::has_selected_unit_v<derived_kind>);
  STATIC_ASSERT_TRUE(std::is_same<arene::base::selected_unit_for_t<derived_kind>, default_unit>::value);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<my_kind_with_default_unit, derived_kind>);
}

/// @test A type derived from a quantity kind is not itself a quantity kind
TEST(QuantityKinds, DerivedClassesAreNotQuantityKinds) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class derived : public my_kind {};

  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<my_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<derived>);
}

/// @test A type privately derived from a @c quantity_kind is not a quantity kind
TEST(QuantityKinds, PrivateInheritanceMeansNotAQuantityKind) {
#if ARENE_IS_ON(ARENE_COMPILER_CLANG)
  class my_kind : private arene::base::quantity_kind<my_kind> {};

  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<my_kind>);
#else
  GTEST_SKIP() << "GCC doesn't handle SFINAE based on access";
#endif
}

/// @test A type privately derived from a @c quantity_kind is not a quantity kind
TEST(QuantityKinds, WrongTagMeansNotAQuantityKind) {
  class other_class {};
  class my_kind : public arene::base::quantity_kind<other_class> {};

  STATIC_ASSERT_FALSE(arene::base::is_quantity_kind_v<my_kind>);
}

/// @test The "units for" a derived kind with a specified default unit are that unit, even if the parent kind has a
/// different default unit
TEST(QuantityKinds, UnitsForDerivedKindWithParentHavingDifferentDefaultUnit) {
  class my_kind_with_default_unit;  // IWYU pragma: keep
  class default_unit : public arene::base::quantity_unit<default_unit, my_kind_with_default_unit> {};
  class my_kind_with_default_unit : public arene::base::quantity_kind<my_kind_with_default_unit, default_unit> {};

  class derived_kind;  // IWYU pragma: keep
  class other_unit : public arene::base::quantity_unit<other_unit, derived_kind> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, my_kind_with_default_unit, other_unit> {};

  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<derived_kind>);
  STATIC_ASSERT_TRUE(arene::base::has_selected_unit_v<derived_kind>);
  STATIC_ASSERT_TRUE(std::is_same<arene::base::selected_unit_for_t<derived_kind>, other_unit>::value);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<my_kind_with_default_unit, derived_kind>);
}

/// @test A unit for a quantity kind is also a unit for any derived quantity kind with that as an ultimate parent
TEST(QuantityKinds, UnitsForAQuantityKindAreAlsoUnitsForDerivedKinds) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, my_kind> {};
  class second_derived_kind : public arene::base::quantity_kind<second_derived_kind, derived_kind> {};

  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<my_unit, derived_kind>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<my_unit, second_derived_kind>);
}

/// @test A derived kind can specify a unit for its parent kind as the default unit
TEST(QuantityKinds, ADerivedKindCanHaveADefaultUnitIntendedForParent) {
  class my_kind : public arene::base::quantity_kind<my_kind> {};
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  class derived_kind : public arene::base::quantity_kind<derived_kind, my_kind, my_unit> {};

  STATIC_ASSERT_TRUE(arene::base::has_selected_unit_v<derived_kind>);
  STATIC_ASSERT_TRUE(std::is_same<arene::base::selected_unit_for_t<derived_kind>, my_unit>::value);
}

/// @test The "units for" a kind is the same regardless of CV qualifiers
TEST(QuantityKinds, UnitsForIgnoresCvQualifiers) {
  class my_kind_with_default_unit;  // IWYU pragma: keep
  class default_unit : public arene::base::quantity_unit<default_unit, my_kind_with_default_unit> {};
  class my_kind_with_default_unit : public arene::base::quantity_kind<my_kind_with_default_unit, default_unit> {};

  STATIC_ASSERT_TRUE(std::is_same<arene::base::selected_unit_for_t<my_kind_with_default_unit>, default_unit>::value);
  STATIC_ASSERT_TRUE(
      std::is_same<arene::base::selected_unit_for_t<my_kind_with_default_unit const>, default_unit>::value
  );
  STATIC_ASSERT_TRUE(
      std::is_same<arene::base::selected_unit_for_t<my_kind_with_default_unit volatile>, default_unit>::value
  );
  STATIC_ASSERT_TRUE(
      std::is_same<arene::base::selected_unit_for_t<my_kind_with_default_unit const volatile>, default_unit>::value
  );
}

/// @test Unrelated types which are not from this library do not register as being or having quantity origins
TEST(QuantityKinds, OriginTraitsAreFalseForUnrelatedTypes) {
  STATIC_ASSERT_FALSE(arene::base::is_quantity_origin_v<int>);
  STATIC_ASSERT_FALSE(arene::base::quantity_has_origin_v<int>);

  STATIC_ASSERT_FALSE(arene::base::is_quantity_origin_v<float>);
  STATIC_ASSERT_FALSE(arene::base::quantity_has_origin_v<float>);

  class local {};
  STATIC_ASSERT_FALSE(arene::base::is_quantity_origin_v<local>);
  STATIC_ASSERT_FALSE(arene::base::quantity_has_origin_v<local>);
}

/// @test It's possible to define a quantity origin
TEST(QuantityKinds, CanDeclareOrigin) {
  class my_origin : public arene::base::quantity_origin<my_origin> {};

  STATIC_ASSERT_TRUE(arene::base::is_quantity_origin_v<my_origin>);
  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<my_origin>);  // An origin is considered to have an origin
}

/// @test It's possible to declare a quantity kind that has an origin and nothing else
TEST(QuantityKinds, CanDeclareQuantityKindWithAnOrigin) {
  class my_origin : public arene::base::quantity_origin<my_origin> {};
  class my_kind : public arene::base::quantity_kind<my_kind, my_origin> {};

  STATIC_ASSERT_FALSE(arene::base::is_quantity_origin_v<my_kind>);
  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<my_kind>);
  ::testing::StaticAssertTypeEq<arene::base::quantity_origin_t<my_kind>, my_origin>();
}

/// @test It's possible to manually add an origin to a quantity kind without inheriting it
TEST(QuantityKinds, CanManuallyAddOriginToQuantityKind) {
  class my_origin : public arene::base::quantity_origin<my_origin> {};
  class my_kind : public arene::base::quantity_kind<my_kind, my_origin> {
   public:
    using origin = my_origin;
  };

  STATIC_ASSERT_FALSE(arene::base::is_quantity_origin_v<my_kind>);
  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<my_kind>);
  ::testing::StaticAssertTypeEq<arene::base::quantity_origin_t<my_kind>, my_origin>();
}

/// @test It's possible to declare a quantity kind that has an origin and a default unit
TEST(QuantityKinds, CanDeclareQuantityKindWithAnOriginAndDefaultUnit) {
  class my_kind;  // IWYU pragma: keep
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  class my_origin : public arene::base::quantity_origin<my_origin> {};
  class my_kind : public arene::base::quantity_kind<my_kind, my_origin, my_unit> {};

  STATIC_ASSERT_FALSE(arene::base::is_quantity_origin_v<my_kind>);
  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<my_kind>);
  ::testing::StaticAssertTypeEq<arene::base::quantity_origin_t<my_kind>, my_origin>();
  ::testing::StaticAssertTypeEq<arene::base::selected_unit_for_t<my_kind>, my_unit>();
}

/// @test It's possible to declare a quantity kind that has a parent, an origin, and a default unit
TEST(QuantityKinds, CanDeclareQuantityKindWithAParentAndOriginAndDefaultUnit) {
  class parent_kind : public arene::base::quantity_kind<parent_kind> {};
  class my_kind;  // IWYU pragma: keep
  class my_unit : public arene::base::quantity_unit<my_unit, my_kind> {};
  class my_origin : public arene::base::quantity_origin<my_origin> {};
  class my_kind : public arene::base::quantity_kind<my_kind, parent_kind, my_origin, my_unit> {};

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<parent_kind, my_kind>);
  STATIC_ASSERT_FALSE(arene::base::is_quantity_origin_v<my_kind>);
  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<my_kind>);
  ::testing::StaticAssertTypeEq<arene::base::quantity_origin_t<my_kind>, my_origin>();
  ::testing::StaticAssertTypeEq<arene::base::selected_unit_for_t<my_kind>, my_unit>();
}

/// @test A derived kind whose parent has an origin has the same origin as its parent
TEST(QuantityKinds, OriginsAreInherited) {
  class my_origin : public arene::base::quantity_origin<my_origin> {};
  class parent_kind : public arene::base::quantity_kind<parent_kind, my_origin> {};
  class my_kind : public arene::base::quantity_kind<my_kind, parent_kind> {};

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<parent_kind, my_kind>);

  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<parent_kind>);
  ::testing::StaticAssertTypeEq<arene::base::quantity_origin_t<parent_kind>, my_origin>();

  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<my_kind>);
  ::testing::StaticAssertTypeEq<arene::base::quantity_origin_t<my_kind>, my_origin>();
}

/// @test A derived kind that explicitly declares the same origin as its parent has that origin
TEST(QuantityKinds, DerivedKindsCanDeclareSameOrigin) {
  class my_origin : public arene::base::quantity_origin<my_origin> {};
  class parent_kind : public arene::base::quantity_kind<parent_kind, my_origin> {};
  class my_kind : public arene::base::quantity_kind<my_kind, parent_kind, my_origin> {};

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<parent_kind, my_kind>);

  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<parent_kind>);
  ::testing::StaticAssertTypeEq<arene::base::quantity_origin_t<parent_kind>, my_origin>();

  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<my_kind>);
  ::testing::StaticAssertTypeEq<arene::base::quantity_origin_t<my_kind>, my_origin>();
}

/// @test A derived kind whose parent has an origin can declare an origin relative to that of its parent
TEST(QuantityKinds, DerivedKindCanHaveRelativeOrigin) {
  class base_origin : public arene::base::quantity_origin<base_origin> {};
  class parent_kind : public arene::base::quantity_kind<parent_kind, base_origin> {};

  class second_origin : public arene::base::relative_origin<second_origin, base_origin> {};
  class second_kind : public arene::base::quantity_kind<second_kind, parent_kind, second_origin> {};

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<parent_kind, second_kind>);

  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<parent_kind>);
  ::testing::StaticAssertTypeEq<arene::base::quantity_origin_t<parent_kind>, base_origin>();

  STATIC_ASSERT_TRUE(arene::base::is_quantity_origin_v<second_origin>);
  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<second_kind>);
  ::testing::StaticAssertTypeEq<arene::base::quantity_origin_t<second_kind>, second_origin>();
}

/// @test An origin can be relative to another relative origin, forming a hierarchy
TEST(QuantityKinds, DerivedKindCanHaveDoublyRelativeOrigin) {
  class base_origin : public arene::base::quantity_origin<base_origin> {};
  class parent_kind : public arene::base::quantity_kind<parent_kind, base_origin> {};

  class second_origin : public arene::base::relative_origin<second_origin, base_origin> {};
  class second_kind : public arene::base::quantity_kind<second_kind, parent_kind, second_origin> {};

  class third_origin : public arene::base::relative_origin<third_origin, second_origin> {};
  class third_kind : public arene::base::quantity_kind<third_kind, second_kind, third_origin> {};

  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<parent_kind, second_kind>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<second_kind, third_kind>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<parent_kind, third_kind>);

  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<parent_kind>);
  ::testing::StaticAssertTypeEq<arene::base::quantity_origin_t<parent_kind>, base_origin>();

  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<second_kind>);
  ::testing::StaticAssertTypeEq<arene::base::quantity_origin_t<second_kind>, second_origin>();

  STATIC_ASSERT_TRUE(arene::base::is_quantity_origin_v<third_origin>);
  STATIC_ASSERT_TRUE(arene::base::quantity_has_origin_v<third_kind>);
  ::testing::StaticAssertTypeEq<arene::base::quantity_origin_t<third_kind>, third_origin>();
}

}  // namespace
