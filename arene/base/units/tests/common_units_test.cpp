// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "arene/base/units/common_units.hpp"

#include <gtest/gtest.h>

#include "arene/base/testing/gtest.hpp"
#include "arene/base/units.hpp"

namespace {
/// @test Check for existence of base quantity kinds
TEST(CommonUnits, BaseQuantityKinds) {
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<arene::base::units::number>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<arene::base::units::length>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<arene::base::units::mass>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<arene::base::units::time>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<arene::base::units::electric_current>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<arene::base::units::temperature>);
  testing::StaticAssertTypeEq<arene::base::units::temperature, arene::base::units::thermodynamic_temperature>();
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<arene::base::units::amount_of_substance>);
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<arene::base::units::luminous_intensity>);
}

/// @test Check for existence of base units
TEST(CommonUnits, BaseQuantityUnits) {
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::meter, arene::base::units::length>);
  testing::StaticAssertTypeEq<arene::base::selected_unit_for_t<arene::base::units::length>, arene::base::units::meter>(
  );

  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::kilogram, arene::base::units::mass>);
  testing::StaticAssertTypeEq<arene::base::selected_unit_for_t<arene::base::units::mass>, arene::base::units::kilogram>(
  );
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::second, arene::base::units::time>);
  testing::StaticAssertTypeEq<arene::base::selected_unit_for_t<arene::base::units::time>, arene::base::units::second>();
  STATIC_ASSERT_TRUE(arene::base::
                         is_possible_unit_for_v<arene::base::units::ampere, arene::base::units::electric_current>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::kelvin, arene::base::units::temperature>);
  testing::
      StaticAssertTypeEq<arene::base::selected_unit_for_t<arene::base::units::temperature>, arene::base::units::kelvin>(
      );
  testing::StaticAssertTypeEq<
      arene::base::quantity_origin_t<arene::base::units::temperature>,
      arene::base::units::absolute_zero>();
  STATIC_ASSERT_TRUE(arene::base::
                         is_possible_unit_for_v<arene::base::units::mole, arene::base::units::amount_of_substance>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_possible_unit_for_v<arene::base::units::candela, arene::base::units::luminous_intensity>);
}

/// @test Other time units
TEST(CommonUnits, OtherTimeUnits) {
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::minute, arene::base::units::time>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::hour, arene::base::units::time>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::millisecond, arene::base::units::time>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::microsecond, arene::base::units::time>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::nanosecond, arene::base::units::time>);
}

/// @test Other length units
TEST(CommonUnits, OtherLengthUnits) {
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::kilometer, arene::base::units::length>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::mile, arene::base::units::length>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::millimeter, arene::base::units::length>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::centimeter, arene::base::units::length>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::micrometer, arene::base::units::length>);
}

/// @test Other mass units
TEST(CommonUnits, OtherMassUnits) {
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::kilogram, arene::base::units::mass>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::milligram, arene::base::units::mass>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<arene::base::units::microgram, arene::base::units::mass>);
}

/// @test Other electric current units
TEST(CommonUnits, OtherElectricCurrentUnits) {
  STATIC_ASSERT_TRUE(arene::base::
                         is_possible_unit_for_v<arene::base::units::amp, arene::base::units::electric_current>);
  testing::StaticAssertTypeEq<arene::base::units::amp, arene::base::units::ampere>();
  STATIC_ASSERT_TRUE(arene::base::
                         is_possible_unit_for_v<arene::base::units::milliamp, arene::base::units::electric_current>);
}

/// @test Angles
TEST(CommonUnits, Angles) {
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<arene::base::units::angular_measure>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_base_quantity_kind_of_v<arene::base::units::number, arene::base::units::angular_measure>);
  testing::StaticAssertTypeEq<arene::base::units::angular_measure, arene::base::units::plane_angle>();
  testing::StaticAssertTypeEq<
      arene::base::selected_unit_for_t<arene::base::units::angular_measure>,
      arene::base::units::radian>();
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<
                     arene::base::units::angular_degree,
                     arene::base::units::angular_measure>);
}

}  // namespace
