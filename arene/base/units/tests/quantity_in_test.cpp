// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/units.hpp"

namespace {

class length : public arene::base::quantity_kind<length> {};
class metre : public arene::base::quantity_unit<metre, length> {};
class mile : public arene::base::quantity_unit<mile, length> {};

class time : public arene::base::quantity_kind<time> {};
class seconds : public arene::base::quantity_unit<seconds, time> {};

/// @test @c in cannot be used if the provided type is not a unit
TEST(QuantityIn, RequiresUnit) {
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<length::in, mile>);
  STATIC_ASSERT_TRUE(!arene::base::substitution_succeeds<length::in, int>);
  STATIC_ASSERT_TRUE(!arene::base::substitution_succeeds<length::in, length>);
  STATIC_ASSERT_TRUE(!arene::base::substitution_succeeds<length::in, mile*>);
  STATIC_ASSERT_TRUE(!arene::base::substitution_succeeds<length::in, mile&>);
  STATIC_ASSERT_TRUE(!arene::base::substitution_succeeds<length::in, mile&&>);
}

/// @test @c in cannot be used if the provided type is not a valid unit for the quantity kind
TEST(QuantityIn, RequiresValidUnit) {
  STATIC_ASSERT_TRUE(arene::base::is_unit_v<seconds>);

  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<length::in, mile>);
  STATIC_ASSERT_TRUE(!arene::base::substitution_succeeds<length::in, seconds>);
}

/// @test @c in can be used to create a kind a unit
TEST(QuantityIn, CreateKindWithUnit) {
  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<length::in<mile>>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<mile, length::in<mile>>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<length, length::in<mile>>);

  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<length::in<metre>>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<metre, length::in<metre>>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<length, length::in<metre>>);

  using length_in_metre = length::in<metre>;

  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<length_in_metre::in<mile>>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<mile, length_in_metre::in<mile>>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<length, length_in_metre::in<mile>>);
}

/// @test @c in used with the existing unit type specifies a different kind
TEST(QuantityIn, CreatesUniqueKind) {
  using length_in_mile = length::in<mile>;

  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<length_in_mile::in<mile>>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<mile, length_in_mile::in<mile>>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<length_in_mile, length_in_mile::in<mile>>);
  STATIC_ASSERT_TRUE(!std::is_same<length_in_mile, length_in_mile::in<mile>>::value);
}

/// @test @c in can be used if the unit's kind is a parent type of the kind
TEST(QuantityIn, UsableIfUnitKindIsAParent) {
  class reach : public arene::base::quantity_kind<reach, length> {};
  class inch : public arene::base::quantity_unit<inch, length> {};

  STATIC_ASSERT_TRUE(arene::base::is_quantity_kind_v<reach::in<inch>>);
  STATIC_ASSERT_TRUE(arene::base::is_possible_unit_for_v<inch, reach::in<inch>>);
  STATIC_ASSERT_TRUE(arene::base::is_base_quantity_kind_of_v<reach, reach::in<inch>>);
}

/// @test @c in cannot be used if the unit's kind is a child type of the kind
TEST(QuantityIn, NotUsableIfUnitKindIsAChild) {
  class reach : public arene::base::quantity_kind<reach, length> {};
  class inch : public arene::base::quantity_unit<inch, reach> {};

  STATIC_ASSERT_TRUE(!arene::base::substitution_succeeds<length::in, inch>);
}

}  // namespace
