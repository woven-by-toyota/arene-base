// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/units.hpp"
#include "arene/base/units/quantity.hpp"

namespace {

class length : public arene::base::quantity_kind<length> {};
class width : public arene::base::quantity_kind<width, length> {};

class time : public arene::base::quantity_kind<time> {};

class miles : public arene::base::quantity_unit<miles, length> {};
class hours : public arene::base::quantity_unit<hours, time> {};

using arene::base::combine_kinds_t;
using arene::base::quantity;

template <class FromKind, class ToKind>
constexpr auto is_quantity_of_convertible_v = std::is_convertible<quantity<FromKind>, quantity<ToKind>>::value;

/// @test quantities of explicit quantity kind types types are implictly convertible
TEST(QuantityImplicitlyConvertible, WithExplicitQuantityKindTypes) {
  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<length, length>);
  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<width, width>);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<width, length>);
  STATIC_ASSERT_FALSE(is_quantity_of_convertible_v<length, width>);
}

/// @test quantities of implicit quantity kind types types are implictly convertible
TEST(QuantityImplicitlyConvertible, WithImplicitQuantityKindTypes) {
  STATIC_ASSERT_FALSE(is_quantity_of_convertible_v<  //
                      length::with_exponent<2>,      //
                      length::with_exponent<1>       //
                      >);
  STATIC_ASSERT_FALSE(is_quantity_of_convertible_v<  //
                      length::with_exponent<2>,      //
                      length::with_exponent<3>       //
                      >);
  STATIC_ASSERT_FALSE(is_quantity_of_convertible_v<  //
                      length::with_exponent<2>,      //
                      length::with_exponent<-2>      //
                      >);
  STATIC_ASSERT_FALSE(is_quantity_of_convertible_v<  //
                      length::with_exponent<2>,      //
                      time::with_exponent<2>         //
                      >);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<  //
                     length::with_exponent<2>,      //
                     length::with_exponent<2>       //
                     >);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<  //
                     length::with_exponent<-2>,     //
                     length::with_exponent<-2>      //
                     >);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<  //
                     width::with_exponent<2>,       //
                     length::with_exponent<2>       //
                     >);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<  //
                     width::with_exponent<-2>,      //
                     length::with_exponent<-2>      //
                     >);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<     //
                     combine_kinds_t<length, length>,  //
                     combine_kinds_t<length, length>   //
                     >);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<    //
                     combine_kinds_t<width, width>,   //
                     combine_kinds_t<length, length>  //
                     >);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<                     //
                     combine_kinds_t<width, time::with_exponent<-1>>,  //
                     combine_kinds_t<length, time::with_exponent<-1>>  //
                     >);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<                     //
                     combine_kinds_t<width, time::with_exponent<-2>>,  //
                     combine_kinds_t<length, time::with_exponent<-2>>  //
                     >);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<                                       //
                     combine_kinds_t<width, length, time::with_exponent<-1>>,            //
                     combine_kinds_t<length::with_exponent<2>, time::with_exponent<-1>>  //
                     >);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<   //
                     combine_kinds_t<width, hours>,  //
                     combine_kinds_t<length, time>   //
                     >);
}

/// @test quantities of implicit quantity kind types are convertible to
/// a target quantity if the target's kind is explicit but has an implicit parent
TEST(QuantityImplicitlyConvertible, ExplicitDestWithImplicitParent) {
  using implicit_speed = combine_kinds_t<length, time::with_exponent<-1>>;
  class explicit_speed : public arene::base::quantity_kind<explicit_speed, implicit_speed> {};
  class explicit_explicit_speed : public arene::base::quantity_kind<explicit_explicit_speed, explicit_speed> {};

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<                     //
                     combine_kinds_t<width, time::with_exponent<-1>>,  //
                     implicit_speed                                    //
                     >);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<                     //
                     combine_kinds_t<width, time::with_exponent<-1>>,  //
                     explicit_speed                                    //
                     >);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<  //
                     implicit_speed,                //
                     explicit_speed                 //
                     >);

  STATIC_ASSERT_FALSE(is_quantity_of_convertible_v<  //
                      implicit_speed,                //
                      explicit_explicit_speed        //
                      >);
}

/// @test quantities with "canceled" units are not implicitly convertible to a quantity that omits the "canceled" units
TEST(QuantityImplicitlyConvertible, QuantityCancelationDoesNotAllowImplicitConversion) {
  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<miles, miles>);

  STATIC_ASSERT_FALSE(is_quantity_of_convertible_v<
                      combine_kinds_t<miles::with_exponent<2>, miles::with_exponent<-1>>,
                      miles>);

  STATIC_ASSERT_FALSE(is_quantity_of_convertible_v<  //
                      combine_kinds_t<miles, time, time::with_exponent<-1>>,
                      miles>);
}

/// @test quantities where a kind can be mapped to a base kind allow implicit conversion
TEST(QuantityImplicitlyConvertible, CombinedDerivedQuantities) {
  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<  //
                     combine_kinds_t<length, length>,
                     combine_kinds_t<length, length>>);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<  //
                     combine_kinds_t<miles, miles>,
                     combine_kinds_t<miles, miles>>);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<  //
                     miles::with_exponent<2>,       //
                     miles::with_exponent<2>>);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<  //
                     length::with_exponent<2>,      //
                     length::with_exponent<2>>);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<  //
                     miles::with_exponent<2>,       //
                     length::with_exponent<2>>);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<    //
                     combine_kinds_t<length, miles>,  //
                     length::with_exponent<2>>);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<  //
                     length::with_exponent<-2>,     //
                     length::with_exponent<-2>>);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<  //
                     miles::with_exponent<-2>,      //
                     length::with_exponent<-2>>);

  STATIC_ASSERT_TRUE(is_quantity_of_convertible_v<   //
                     combine_kinds_t<                //
                         length::with_exponent<-1>,  //
                         miles::with_exponent<-1>>,  //
                     length::with_exponent<-2>>);

  STATIC_ASSERT_FALSE(is_quantity_of_convertible_v<  //
                      length::with_exponent<2>,      //
                      miles::with_exponent<2>>);
}

}  // namespace
