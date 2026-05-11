// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/units/quantity.hpp"

#include <gtest/gtest.h>

#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/stdlib_choice/ratio.hpp"
#include "arene/base/testing/gtest.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/units/units_conversion_traits.hpp"

namespace {

template <typename Q, typename... T>
using count_of_result_t = decltype(std::declval<Q>().template count_of<T...>());

template <typename Q, typename... T>
using as_result_t = decltype(std::declval<Q>().template as<T...>());

using arene::base::quantity;

template <typename Q1, typename Q2>
using add_result_t = decltype(std::declval<Q1>() + std::declval<Q2>());

template <typename Q1, typename Q2>
using subtract_result_t = decltype(std::declval<Q1>() - std::declval<Q2>());

template <typename Q1, typename Q2>
using multiply_result_t = decltype(std::declval<Q1>() * std::declval<Q2>());

template <typename Q1, typename Q2>
using divide_result_t = decltype(std::declval<Q1>() / std::declval<Q2>());

/// @test A default-constructed `quantity` has a count of zero
TEST(Quantity, ADefaultConstructedQuantityHasACountOfZero) {
  struct unit {};
  constexpr quantity<unit, std::uint32_t> qty{};

  STATIC_ASSERT_EQ(qty.count_of<unit>(), 0);
  STATIC_ASSERT_TRUE(std::is_same<count_of_result_t<quantity<unit, std::uint32_t>, unit>, std::uint32_t>::value);
  STATIC_ASSERT_TRUE(std::is_same<count_of_result_t<quantity<unit, double>, unit>, double>::value);
  STATIC_ASSERT_TRUE(std::is_same<count_of_result_t<quantity<unit, std::int16_t>, unit>, std::int16_t>::value);
}

/// @test Invoking `count_of` on a `quantity` constructed with a count returns that count
TEST(Quantity, IfConstructedWithACountThenCountOfReturnsThat) {
  struct unit {};
  constexpr std::uint32_t count = 123;
  constexpr quantity<unit, std::uint32_t> qty{count};

  STATIC_ASSERT_EQ(qty.count_of<unit>(), count);
}

/// @test Attmepting to invoke `count_of` passing an incompatible unit type will not compile
TEST(Quantity, CannotGetCountOfForIncompatibleUnits) {
  struct unit1 {};
  struct unit2 {};

  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<count_of_result_t, quantity<unit1, std::uint32_t>, unit2>);
}

struct foo_unit {};
struct bar_unit {};

constexpr std::uint32_t foobar_numerator = 25;
constexpr std::uint32_t foobar_denominator = 17;

}  // namespace

template <>
struct arene::base::units_conversion_traits<foo_unit, bar_unit> {
  ARENE_MAYBE_UNUSED static constexpr bool compatible = true;

  using scale_factor = std::ratio<foobar_numerator, foobar_denominator>;
};

namespace {

/// @test Invoking `count_of` with a unit type that is not the same as the `quantity`'s unit type but is compatible
/// returns the scaled value of the count as per the compatibility traits
TEST(Quantity, CanGetCountOfForCompatibleUnits) {
  STATIC_ASSERT_TRUE(arene::base::
                         substitution_succeeds<count_of_result_t, quantity<foo_unit, std::uint32_t>, bar_unit>);

  constexpr std::uint32_t count = 123;
  constexpr quantity<foo_unit, std::uint32_t> qty{count};
  STATIC_ASSERT_EQ(qty.count_of<bar_unit>(), count * foobar_numerator / foobar_denominator);
}

struct tiny_unit {};

constexpr std::int64_t footiny_numerator = std::numeric_limits<std::int64_t>::max() - 5;
constexpr std::int64_t footiny_denominator = std::numeric_limits<std::int64_t>::max();
}  // namespace
template <>
struct arene::base::units_conversion_traits<foo_unit, tiny_unit> {
  ARENE_MAYBE_UNUSED static constexpr bool compatible = true;

  using scale_factor = std::ratio<footiny_numerator, footiny_denominator>;
};

namespace {

/// @test Invoking `count_of` with a unit type that is not the same as the `quantity`'s unit type but is compatible
/// returns the scaled value of the count as per the compatibility traits, when using a `double` as the representation
TEST(Quantity, GetCountOfWithDouble) {
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<count_of_result_t, quantity<foo_unit, double>, tiny_unit>);

  constexpr auto count = static_cast<double>(footiny_denominator);
  constexpr quantity<foo_unit, double> qty{count};
  constexpr auto tiny_count = qty.count_of<tiny_unit>();
  ASSERT_NEAR(
      tiny_count,
      count * static_cast<double>(footiny_numerator) / static_cast<double>(footiny_denominator),
      1e-300
  );

  constexpr double count2 = 1717.17;
  constexpr quantity<foo_unit, double> qty2{count2};
  constexpr auto bar_count = qty2.count_of<bar_unit>();
  ASSERT_NEAR(bar_count, 2525.25, 1e-300);
}

/// @test A `quantity` can be converted to a `quantity` with a different unit type, by invoking the `as` member
/// function, and invoking `count_of` with the new unit type on the new `quantity` yields a value scaled as per the
/// compatibility traits
TEST(Quantity, CanGetAlternativeQuantityTypeWithAs) {
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<as_result_t, quantity<foo_unit, std::uint32_t>, bar_unit>);
  STATIC_ASSERT_TRUE(
      std::is_same<as_result_t<quantity<foo_unit, std::uint32_t>, bar_unit>, quantity<bar_unit, std::uint32_t>>::value
  );

  constexpr std::uint32_t count = 123;
  constexpr quantity<foo_unit, std::uint32_t> qty{count};
  constexpr quantity<bar_unit, std::uint32_t> qty2 = qty.as<bar_unit>();
  STATIC_ASSERT_EQ(qty2.count_of<bar_unit>(), count * foobar_numerator / foobar_denominator);
}

/// @test A `quantity` can be converted to a `quantity` with a different unit type and representation, by invoking the
/// `as` member function, and invoking `count_of` with the new unit type on the new `quantity` yields a value scaled as
/// per the compatibility traits, in the new representation type
TEST(Quantity, CanGetAlternativeQuantityTypeAndRepWithAs) {
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<
                     as_result_t,
                     quantity<foo_unit, std::uint32_t>,
                     bar_unit,
                     std::uint64_t>);
  STATIC_ASSERT_TRUE(std::is_same<
                     as_result_t<quantity<foo_unit, std::uint32_t>, bar_unit, std::uint64_t>,
                     quantity<bar_unit, std::uint64_t>>::value);

  constexpr std::uint32_t count = 123;
  constexpr quantity<foo_unit, std::uint32_t> qty{count};
  constexpr quantity<bar_unit, std::uint64_t> qty2 = qty.as<bar_unit, std::uint64_t>();
  STATIC_ASSERT_EQ(qty2.count_of<bar_unit>(), count * foobar_numerator / foobar_denominator);
}

/// @test Attempting implicit conversion of a `quantity` with one unit type to a `quantity` with a different unit type
/// does not compile
TEST(Quantity, ImplicitConversionNotSupportedForIncompatibleTypes) {
  struct unit1 {};
  struct unit2 {};
  STATIC_ASSERT_FALSE(std::is_convertible<quantity<unit1, double>, quantity<unit2, double>>::value);
}

/// @test A `quantity` with one unit type can be explictily (not implicitly) converted to a `quantity` with a different
/// unit type with a representation type with a smaller range if those types are "compatible", with the result having a
/// count scaled as per the compatibility traits
TEST(Quantity, ExplicitConversionOnlyForCompatibleTypesWithSmallerRep) {
  STATIC_ASSERT_FALSE(std::is_convertible<quantity<foo_unit, std::uint32_t>, quantity<bar_unit, std::uint16_t>>::value);

  constexpr std::uint32_t count = 123;
  constexpr quantity<foo_unit, std::uint32_t> qty{count};
  constexpr auto qty2 = qty.as<bar_unit, std::uint16_t>();
  STATIC_ASSERT_EQ(qty2.count_of<bar_unit>(), count * foobar_numerator / foobar_denominator);
}

/// @test A `quantity` with one unit type can be implicitly converted to a `quantity` with a different unit type with a
/// representation type with a larger range if those types are "compatible", with the result having a count scaled as
/// per the compatibility traits
TEST(Quantity, ImplicitConversionSupportedForCompatibleTypes) {
  STATIC_ASSERT_TRUE(std::is_convertible<quantity<foo_unit, std::uint32_t>, quantity<bar_unit, std::uint64_t>>::value);

  constexpr std::uint32_t count = 123;
  constexpr quantity<foo_unit, std::uint32_t> qty{count};
  constexpr quantity<bar_unit, std::uint64_t> qty2 = qty;
  STATIC_ASSERT_EQ(qty2.count_of<bar_unit>(), count * foobar_numerator / foobar_denominator);
}

/// @test A `quantity` can be implicitly converted to a `quantity` with the same unit type with a representation type
/// with a larger range
TEST(Quantity, ImplicitConversionSupportedForSameQuantityLargerRep) {
  STATIC_ASSERT_TRUE(std::is_convertible<quantity<foo_unit, std::uint8_t>, quantity<foo_unit, std::uint32_t>>::value);

  constexpr std::uint32_t count = 123;
  constexpr quantity<foo_unit, std::uint8_t> qty{count};
  constexpr quantity<foo_unit, std::uint32_t> qty2 = qty;
  STATIC_ASSERT_EQ(qty2.count_of<foo_unit>(), count);
}

/// @test A `quantity` cannot be implicitly converted to a `quantity` with the same unit type with a representation type
/// with a smaller range
TEST(Quantity, ImplicitConversionNotSupportedForSameQuantitySmallerRep) {
  STATIC_ASSERT_FALSE(std::is_convertible<quantity<foo_unit, std::uint32_t>, quantity<foo_unit, std::uint8_t>>::value);
}

struct baz_unit {};

constexpr std::uint32_t foobaz_numerator = 1;
constexpr std::uint32_t foobaz_denominator = 256;
}  // namespace
template <>
struct arene::base::units_conversion_traits<foo_unit, baz_unit> {
  ARENE_MAYBE_UNUSED static constexpr bool compatible = true;

  using scale_factor = std::ratio<foobaz_numerator, foobaz_denominator>;
};

namespace {

/// @test A `quantity` can be implicitly converted to a `quantity` with a different unit type with a representation
/// type with a smaller range, if the types are compatible, and the scaling specified by the compatibility traits means
/// the value is guaranteed to fit in the smallest type
TEST(Quantity, ImplicitConversionSupportedForSmallerRepWhenValueFits) {
  STATIC_ASSERT_TRUE(std::is_convertible<quantity<foo_unit, std::uint16_t>, quantity<baz_unit, std::uint8_t>>::value);

  constexpr std::uint32_t count = 123;
  constexpr quantity<foo_unit, std::uint16_t> qty{count};
  constexpr quantity<baz_unit, std::uint8_t> qty2 = qty;
  STATIC_ASSERT_EQ(qty2.count_of<baz_unit>(), count * foobaz_numerator / foobaz_denominator);
}

struct wibble_unit {};

constexpr std::int32_t foowibble_numerator = 256;
constexpr std::int32_t foowibble_denominator = 1;
}  // namespace
template <>
struct arene::base::units_conversion_traits<foo_unit, wibble_unit> {
  ARENE_MAYBE_UNUSED static constexpr bool compatible = true;

  using scale_factor = std::ratio<foowibble_numerator, foowibble_denominator>;
};

namespace {

/// @test A `quantity` can be implicitly converted to a `quantity` with a different unit type with a representation
/// type with a larger range, and the value is scaled as specified by the compatibility traits
TEST(Quantity, ImplicitConversionToLargerRepPreservesValues) {
  STATIC_ASSERT_TRUE(std::is_convertible<quantity<foo_unit, std::uint8_t>, quantity<wibble_unit, std::uint16_t>>::value
  );

  constexpr std::uint32_t count = 123;
  constexpr quantity<foo_unit, std::uint8_t> qty{count};
  constexpr quantity<wibble_unit, std::uint32_t> qty2 = qty;
  STATIC_ASSERT_EQ(qty2.count_of<wibble_unit>(), count * foowibble_numerator / foowibble_denominator);
}

/// @test A `quantity` can be implicitly converted to a `quantity` with a different unit type with a representation
/// type with a larger range, and the value is scaled as specified by the compatibility traits, even for signed numbers
TEST(Quantity, ImplicitConversionToLargerRepPreservesValuesWithSignedNumbers) {
  STATIC_ASSERT_TRUE(std::is_convertible<quantity<foo_unit, std::int8_t>, quantity<wibble_unit, std::int16_t>>::value);

  constexpr std::int32_t count = -123;
  constexpr quantity<foo_unit, std::int8_t> qty{count};
  constexpr quantity<wibble_unit, std::int32_t> qty2 = qty;
  STATIC_ASSERT_EQ(qty2.count_of<wibble_unit>(), count * foowibble_numerator / foowibble_denominator);
}

/// @test A `quantity` can be implicitly converted to a `quantity` with a different unit type with a representation
/// type with a larger range, and the value is scaled as specified by the compatibility traits, even for floating point
TEST(Quantity, ImplicitConversionToLargerRepPreservesValuesWithFloat) {
  STATIC_ASSERT_TRUE(std::is_convertible<quantity<foo_unit, std::int8_t>, quantity<wibble_unit, double>>::value);

  constexpr std::int32_t count = -123;
  constexpr quantity<foo_unit, std::int8_t> qty{count};
  constexpr quantity<wibble_unit, double> qty2 = qty;
  ASSERT_NEAR(
      qty2.count_of<wibble_unit>(),
      static_cast<double>(count * foowibble_numerator) / foowibble_denominator,
      1e-300
  );
}

/// @test A `quantity` with a floating point representation cannot be implicitly converted to a `quantity` with the same
/// unit type with an integer representation
TEST(Quantity, ImplicitConversionDisabledFromFloatToInteger) {
  STATIC_ASSERT_FALSE(std::is_convertible<quantity<foo_unit, double>, quantity<foo_unit, std::int64_t>>::value);
}

/// @test A `quantity` with a floating point representation can be converted to a `quantity` with a different unit
/// type with an integer representation using `as`, and the count is scaled as per the compatibility traits
TEST(Quantity, ExplicitFloatingPointScalingUsingAs) {
  constexpr std::int32_t count = -123;
  constexpr quantity<foo_unit, double> qty{count};
  constexpr quantity<wibble_unit, std::int32_t> qty2 = qty.as<wibble_unit, std::int32_t>();
  STATIC_ASSERT_EQ(qty2.count_of<wibble_unit>(), count * foowibble_numerator / foowibble_denominator);
}

struct derived_unit1 {};
struct derived_unit2 {};
struct common_base_unit {};

constexpr std::int64_t d1_numerator = 42;
constexpr std::int64_t d1_denominator = 33;

constexpr std::int64_t d2_numerator = 13;
constexpr std::int64_t d2_denominator = 43;
}  // namespace
template <>
struct arene::base::units_conversion_traits<derived_unit1, derived_unit2> {
  ARENE_MAYBE_UNUSED static constexpr bool compatible = false;

  using common_type = common_base_unit;
};

template <>
struct arene::base::units_conversion_traits<derived_unit1, common_base_unit> {
  ARENE_MAYBE_UNUSED static constexpr bool compatible = true;

  using scale_factor = std::ratio<d1_numerator, d1_denominator>;
};

template <>
struct arene::base::units_conversion_traits<common_base_unit, derived_unit2> {
  ARENE_MAYBE_UNUSED static constexpr bool compatible = false;

  using common_type = common_base_unit;
  using scale_factor = std::ratio<d2_numerator, d2_denominator>;
};

namespace {

/// @test A `quantity` can be explicitly converted to another `quantity` with a different unit type if the two unit
/// types have a common type
TEST(Quantity, ExplicitConversionOKForIncompatibleTypesWithCommonType) {
  STATIC_ASSERT_TRUE(std::is_convertible<quantity<derived_unit1, double>, quantity<common_base_unit, double>>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<quantity<derived_unit1, double>, quantity<derived_unit2, double>>::value);
  STATIC_ASSERT_FALSE(std::is_convertible<quantity<common_base_unit, double>, quantity<derived_unit2, double>>::value);

  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<as_result_t, quantity<derived_unit1, double>, derived_unit2>);
  STATIC_ASSERT_TRUE(
      std::is_same<as_result_t<quantity<derived_unit1, double>, derived_unit2>, quantity<derived_unit2, double>>::value
  );

  constexpr double count = 12.3;
  constexpr quantity<derived_unit1, double> qty{count};
  constexpr auto qty2 = qty.as<derived_unit2>();
  ASSERT_NEAR(
      qty2.count_of<derived_unit2>(),
      (count * d1_numerator * d2_numerator) / (d1_denominator * d2_denominator),
      1e-300
  );
}

/// @test `quantity` values with unrelated unit types are not comparable
TEST(Quantity, UnrelatedTypesAreNotComparable) {
  struct unit1 {};
  struct unit2 {};
  STATIC_ASSERT_FALSE(arene::base::is_equality_comparable_v<quantity<unit1, double>, quantity<unit2, double>>);
  STATIC_ASSERT_FALSE(arene::base::is_inequality_comparable_v<quantity<unit1, double>, quantity<unit2, double>>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_comparable_v<quantity<unit1, double>, quantity<unit2, double>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_less_than_or_equal_comparable_v<quantity<unit1, double>, quantity<unit2, double>>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<quantity<unit1, double>, quantity<unit2, double>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_greater_than_or_equal_comparable_v<quantity<unit1, double>, quantity<unit2, double>>);
  STATIC_ASSERT_FALSE(arene::base::is_three_way_comparable_v<quantity<unit1, double>, quantity<unit2, double>>);
}

/// @test `quantity` values with incompatible unit types are not comparable
TEST(Quantity, IncompatibleRelatedTypesAreNotComparable) {
  STATIC_ASSERT_FALSE(arene::base::
                          is_equality_comparable_v<quantity<derived_unit1, double>, quantity<derived_unit2, double>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_inequality_comparable_v<quantity<derived_unit1, double>, quantity<derived_unit2, double>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_less_than_comparable_v<quantity<derived_unit1, double>, quantity<derived_unit2, double>>);
  STATIC_ASSERT_FALSE(arene::base::is_less_than_or_equal_comparable_v<
                      quantity<derived_unit1, double>,
                      quantity<derived_unit2, double>>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_comparable_v<
                      quantity<derived_unit1, double>,
                      quantity<derived_unit2, double>>);
  STATIC_ASSERT_FALSE(arene::base::is_greater_than_or_equal_comparable_v<
                      quantity<derived_unit1, double>,
                      quantity<derived_unit2, double>>);
  STATIC_ASSERT_FALSE(arene::base::
                          is_three_way_comparable_v<quantity<derived_unit1, double>, quantity<derived_unit2, double>>);
}

/// @test `quantity` values with the same unit types can be compared with all relational operators, with the expected
/// results
TEST(Quantity, SameTypeIsComparable) {
  struct unit {};
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<quantity<unit, double>, quantity<unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<quantity<unit, double>, quantity<unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<quantity<unit, double>, quantity<unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<quantity<unit, double>, quantity<unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<quantity<unit, double>, quantity<unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_greater_than_or_equal_comparable_v<quantity<unit, double>, quantity<unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::is_three_way_comparable_v<quantity<unit, double>, quantity<unit, double>>);

  using qty = quantity<unit, std::uint32_t>;

  STATIC_ASSERT_EQ(qty{42}, qty{42});
  STATIC_ASSERT_NE(qty{42}, qty{43});
  STATIC_ASSERT_LT(qty{41}, qty{42});
  STATIC_ASSERT_GT(qty{42}, qty{41});
  STATIC_ASSERT_LE(qty{42}, qty{42});
  STATIC_ASSERT_GE(qty{42}, qty{42});
}

/// @test `quantity` values with the same unit types and different representation types can be compared with all
/// relational operators, with the expected results
TEST(Quantity, SameUnitDifferentRepIsComparable) {
  struct unit {};
  using qty1 = quantity<unit, double>;
  using qty2 = quantity<unit, std::uint32_t>;
  using qty3 = quantity<unit, std::uint8_t>;
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<qty1, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<qty2, qty1>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<qty1, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<qty2, qty1>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<qty1, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<qty2, qty1>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<qty1, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<qty2, qty1>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<qty1, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<qty2, qty1>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<qty1, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<qty2, qty1>);
  STATIC_ASSERT_TRUE(arene::base::is_three_way_comparable_v<qty1, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_three_way_comparable_v<qty2, qty1>);

  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<qty3, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<qty2, qty3>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<qty3, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<qty2, qty3>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<qty3, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<qty2, qty3>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<qty3, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<qty2, qty3>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<qty3, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<qty2, qty3>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<qty3, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<qty2, qty3>);
  STATIC_ASSERT_TRUE(arene::base::is_three_way_comparable_v<qty3, qty2>);
  STATIC_ASSERT_TRUE(arene::base::is_three_way_comparable_v<qty2, qty3>);

  STATIC_ASSERT_EQ(qty1{42}, qty2{42});
  STATIC_ASSERT_NE(qty1{42.1}, qty2{42});
  STATIC_ASSERT_LT(qty1{42.9}, qty3{43});
  STATIC_ASSERT_GT(qty1{42.1}, qty2{42});
  STATIC_ASSERT_LE(qty2{42}, qty3{43});
  STATIC_ASSERT_GE(qty3{42}, qty1{41.9});

  STATIC_ASSERT_NE(qty2{256}, qty3{0});
  STATIC_ASSERT_GT(qty2{256}, qty3{255});
}

/// @test Instances of `quantity` with compatible unit types can be compared using all the relational operators.
TEST(Quantity, CompatibleTypesAreComparable) {
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<quantity<foo_unit, double>, quantity<bar_unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<quantity<bar_unit, double>, quantity<foo_unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::is_inequality_comparable_v<quantity<foo_unit, double>, quantity<bar_unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<quantity<foo_unit, double>, quantity<bar_unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<quantity<bar_unit, double>, quantity<foo_unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_less_than_or_equal_comparable_v<quantity<foo_unit, double>, quantity<bar_unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_comparable_v<quantity<foo_unit, double>, quantity<bar_unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_greater_than_or_equal_comparable_v<quantity<foo_unit, double>, quantity<bar_unit, double>>);
}

/// @test Instances of `quantity` with compatible unit types can be compared using all the relational operators, even if
/// the representation types are different.
TEST(Quantity, CompatibleTypesAreComparableEvenWithDifferentRepTypes) {
  STATIC_ASSERT_TRUE(arene::base::
                         is_equality_comparable_v<quantity<foo_unit, double>, quantity<bar_unit, std::uint8_t>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_equality_comparable_v<quantity<foo_unit, std::uint8_t>, quantity<bar_unit, std::uint32_t>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_inequality_comparable_v<quantity<foo_unit, double>, quantity<bar_unit, std::uint8_t>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_less_than_comparable_v<quantity<foo_unit, double>, quantity<bar_unit, std::uint8_t>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_comparable_v<
                     quantity<foo_unit, std::uint8_t>,
                     quantity<bar_unit, std::uint32_t>>);
  STATIC_ASSERT_TRUE(arene::base::is_less_than_or_equal_comparable_v<
                     quantity<foo_unit, double>,
                     quantity<bar_unit, std::uint8_t>>);
  STATIC_ASSERT_TRUE(arene::base::
                         is_greater_than_comparable_v<quantity<foo_unit, double>, quantity<bar_unit, std::uint8_t>>);
  STATIC_ASSERT_TRUE(arene::base::is_greater_than_or_equal_comparable_v<
                     quantity<foo_unit, std::uint32_t>,
                     quantity<bar_unit, double>>);
}

/// @test Instances of `quantity` with compatible unit types can be compared using all the relational operators, even if
/// the representation type of one is too small to hold the converted value of the other
TEST(Quantity, CompatibleTypesAreComparableEvenIfCommonRepIsTooSmall) {
  using foo_qty = quantity<foo_unit, std::int32_t>;
  using bar_qty = quantity<bar_unit, std::int32_t>;

  STATIC_ASSERT_TRUE(arene::base::is_equality_comparable_v<foo_qty, bar_qty>);

  foo_qty qty1{std::numeric_limits<std::int32_t>::max()};
  bar_qty qty2{std::numeric_limits<std::int32_t>::max()};

  ASSERT_GT(qty1, qty2);
  ASSERT_GE(qty1, qty2);
  ASSERT_FALSE(qty1 < qty2);
  ASSERT_FALSE(qty1 <= qty2);
  ASSERT_LT(qty2, qty1);
  ASSERT_LE(qty2, qty1);
  ASSERT_FALSE(qty2 > qty1);
  ASSERT_FALSE(qty2 >= qty1);

  constexpr foo_qty qty3{std::numeric_limits<std::int32_t>::min()};
  constexpr bar_qty qty4{std::numeric_limits<std::int32_t>::min()};

  STATIC_ASSERT_GT(qty4, qty3);
  STATIC_ASSERT_GE(qty4, qty3);
  STATIC_ASSERT_FALSE(qty4 < qty3);
  STATIC_ASSERT_FALSE(qty4 <= qty3);
  STATIC_ASSERT_LT(qty3, qty4);
  STATIC_ASSERT_LE(qty3, qty4);
  STATIC_ASSERT_FALSE(qty3 > qty4);
  STATIC_ASSERT_FALSE(qty3 >= qty4);

  qty1 = foo_qty{42};
  qty2 = qty1.as<bar_unit>();
  ASSERT_EQ(qty2, qty1);
  qty2 = bar_qty{qty2.count_of<bar_unit>() + 1};
  ASSERT_LT(qty1, qty2);
  qty2 = qty4;
  qty1 = foo_qty{
      static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::min()) * foobar_denominator / foobar_numerator - 1
  };
  ASSERT_LE(qty1, qty2);
  qty1 = foo_qty{qty1.count_of<foo_unit>() - 1};
  ASSERT_LT(qty1, qty2);
  qty1 = foo_qty{
      qty1.count_of<foo_unit>() + 1 +
      static_cast<std::int32_t>((foobar_denominator + foobar_numerator - 1) / foobar_numerator)
  };
  ASSERT_GT(qty1, qty2) << "q1=" << qty1.count_of<bar_unit, std::int64_t>() << " q2=" << qty2.count_of<bar_unit>();
}

struct simple_unit {};
struct point_unit {};
}  // namespace
template <>
struct arene::base::units_combination_traits<simple_unit, point_unit> {
  using sum_type = point_unit;
};

template <>
struct arene::base::units_combination_traits<point_unit, simple_unit> {
  using sum_type = point_unit;
};

template <>
struct arene::base::units_combination_traits<point_unit, point_unit> {
  using difference_type = simple_unit;
};

namespace {

/// @test `quantity` objects with a "relative" unit type can be added, and the result is the sum of the values
TEST(Quantity, IdenticalQuantitiesWithRelativeTypesCanBeAdded) {
  using qty = quantity<simple_unit, std::uint32_t>;

  constexpr std::uint32_t count1 = 42;
  constexpr std::uint32_t count2 = 99;
  constexpr qty val1{count1};
  constexpr qty val2{count2};

  constexpr auto sum = val1 + val2;

  STATIC_ASSERT_TRUE(std::is_same<decltype(sum), qty const>::value);
  STATIC_ASSERT_EQ(sum.count_of<simple_unit>(), count1 + count2);
}

/// @test `quantity` objects with a "relative" unit type can be subtracted, and the result is the same type, holding the
/// difference in the values
TEST(Quantity, IdenticalQuantitiesWithRelativeTypesCanBeSubtracted) {
  using qty = quantity<simple_unit, std::int32_t>;

  constexpr std::int32_t count1 = 42;
  constexpr std::int32_t count2 = 99;
  constexpr qty val1{count1};
  constexpr qty val2{count2};

  constexpr auto sum = val1 - val2;

  STATIC_ASSERT_TRUE(std::is_same<decltype(sum), qty const>::value);
  STATIC_ASSERT_EQ(sum.count_of<simple_unit>(), count1 - count2);
}

/// @test `quantity` objects with different "relative" unit types that are compatible can be added, and the result is
/// the type that the conversion is defined for, with a suitable scaling of the other type
TEST(Quantity, CanAddCompatibleUnitsWithAcceptableScaling) {
  constexpr std::uint32_t count1 = 123;
  constexpr quantity<foo_unit, std::uint32_t> qty1{count1};
  constexpr std::uint64_t count2 = 42;
  constexpr quantity<bar_unit, std::uint64_t> qty2{count2};

  STATIC_ASSERT_TRUE(std::is_same<decltype(qty1 + qty2), quantity<bar_unit, std::uint64_t>>::value);
  STATIC_ASSERT_EQ((qty1 + qty2).count_of<bar_unit>(), count1 * foobar_numerator / foobar_denominator + count2);
  STATIC_ASSERT_EQ((qty2 + qty1).count_of<bar_unit>(), count1 * foobar_numerator / foobar_denominator + count2);
}

/// @test `quantity` objects with different "relative" unit types that are compatible can be added, and the result is
/// the type that the conversion is defined for, with a suitable scaling of the other type, with floating point
/// representations
TEST(Quantity, CanAddCompatibleUnitsWithFloatingPointReps) {
  constexpr double count1 = 123.2;
  constexpr quantity<foo_unit, double> qty1{count1};
  constexpr double count2 = 42.6;
  constexpr quantity<bar_unit, double> qty2{count2};

  STATIC_ASSERT_TRUE(std::is_same<decltype(qty1 + qty2), quantity<bar_unit, double>>::value);
  ASSERT_NEAR((qty1 + qty2).count_of<bar_unit>(), count1 * foobar_numerator / foobar_denominator + count2, 1e-300);
  ASSERT_NEAR((qty2 + qty1).count_of<bar_unit>(), count1 * foobar_numerator / foobar_denominator + count2, 1e-300);
}

/// @test `quantity` objects with "point" unit types cannot be added
TEST(Quantity, CannotAddPointUnits) {
  using qty = quantity<point_unit, std::uint32_t>;

  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<add_result_t, qty, qty>);
}

/// @test `quantity` objects with "point" unit types can be added to `quantity` objects with the appropriate
/// "difference" unit type, and the result is a `quantity` with the "point" unit type and a value that is the sum of the
/// values
TEST(Quantity, CanAddPointUnitsToTheirDifferenceType) {
  using qty1 = quantity<point_unit, std::uint32_t>;
  using qty2 = quantity<simple_unit, std::uint32_t>;

  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<add_result_t, qty1, qty2>);
  STATIC_ASSERT_TRUE(std::is_same<add_result_t<qty1, qty2>, qty1>::value);

  constexpr std::int32_t count1 = 42;
  constexpr std::int32_t count2 = 99;
  constexpr qty1 val1{count1};
  constexpr qty2 val2{count2};

  constexpr auto sum = val1 + val2;

  STATIC_ASSERT_TRUE(std::is_same<decltype(sum), qty1 const>::value);
  STATIC_ASSERT_EQ(sum.count_of<point_unit>(), count1 + count2);
}

/// @test `quantity` objects with "point" unit types can be added to `quantity` objects with the appropriate
/// "difference" unit type, with the "difference" quantity on the left hand side, and the result is a `quantity` with
/// the "point" unit type and a value that is the sum of the values
TEST(Quantity, CanAddPointUnitsToTheirDifferenceTypeInReverseOrder) {
  using qty1 = quantity<point_unit, std::uint32_t>;
  using qty2 = quantity<simple_unit, std::uint32_t>;

  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<add_result_t, qty2, qty1>);
  STATIC_ASSERT_TRUE(std::is_same<add_result_t<qty2, qty1>, qty1>::value);

  constexpr std::int32_t count1 = 42;
  constexpr std::int32_t count2 = 99;
  constexpr qty1 val1{count1};
  constexpr qty2 val2{count2};

  constexpr auto sum = val2 + val1;

  STATIC_ASSERT_TRUE(std::is_same<decltype(sum), qty1 const>::value);
  STATIC_ASSERT_EQ(sum.count_of<point_unit>(), count1 + count2);
}

/// @test Two `quantity` objects with the same "point" unit type can be subtracted, and the result is a `quantity` the
/// corresponding "difference" unit type and a value that is the difference between the values
TEST(Quantity, CanSubtractPointUnitsYieldingTheDifferenceType) {
  using qty1 = quantity<point_unit, std::int32_t>;
  using qty2 = quantity<simple_unit, std::int32_t>;

  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<subtract_result_t, qty1, qty1>);
  STATIC_ASSERT_TRUE(std::is_same<subtract_result_t<qty1, qty1>, qty2>::value);

  constexpr std::int32_t count1 = 42;
  constexpr std::int32_t count2 = 99;
  constexpr qty1 val1{count1};
  constexpr qty1 val2{count2};

  constexpr auto sum = val1 - val2;

  STATIC_ASSERT_TRUE(std::is_same<decltype(sum), qty2 const>::value);
  STATIC_ASSERT_EQ(sum.count_of<simple_unit>(), count1 - count2);
}

/// @test `quantity` objects for which multiplication is not enabled cannot be multiplied
TEST(Quantity, CannotMultiplyUnitsNotEnabled) {
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<
                      multiply_result_t,
                      quantity<simple_unit, double>,
                      quantity<simple_unit, double>>);
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<
                      multiply_result_t,
                      quantity<point_unit, double>,
                      quantity<point_unit, double>>);
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<
                      multiply_result_t,
                      quantity<simple_unit, double>,
                      quantity<point_unit, double>>);
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<
                      multiply_result_t,
                      quantity<foo_unit, double>,
                      quantity<foo_unit, double>>);
}

/// @test A `quantity` can be multiplied by a scalar if the result is in the range of the representation
TEST(Quantity, CanMultiplyByScalarIfResultInRange) {
  STATIC_ASSERT_TRUE(arene::base::
                         substitution_succeeds<multiply_result_t, quantity<simple_unit, std::uint32_t>, std::uint32_t>);
  STATIC_ASSERT_TRUE(arene::base::
                         substitution_succeeds<multiply_result_t, std::uint32_t, quantity<simple_unit, std::uint32_t>>);

  using qty = quantity<simple_unit, std::uint32_t>;

  constexpr std::uint32_t val1 = 10;
  constexpr std::uint32_t val2 = 99;
  constexpr std::uint32_t val3 = 42;

  constexpr qty qty1{val1};
  constexpr qty qty2 = qty1 * val2;
  constexpr qty qty3 = val3 * qty2;

  STATIC_ASSERT_EQ(qty2.count_of<simple_unit>(), val1 * val2);
  STATIC_ASSERT_EQ(qty3.count_of<simple_unit>(), val1 * val2 * val3);
}

/// @test A `quantity` can be multiplied by a scalar if the representation type is a floating point type
TEST(Quantity, CanMultiplyByScalarIfResultIsFloat) {
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<multiply_result_t, quantity<simple_unit, double>, double>);
  STATIC_ASSERT_TRUE(arene::base::
                         substitution_succeeds<multiply_result_t, quantity<simple_unit, double>, std::uint32_t>);
  STATIC_ASSERT_TRUE(arene::base::
                         substitution_succeeds<multiply_result_t, quantity<simple_unit, std::uint32_t>, double>);
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<multiply_result_t, double, quantity<simple_unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::
                         substitution_succeeds<multiply_result_t, std::uint32_t, quantity<simple_unit, double>>);
  STATIC_ASSERT_TRUE(arene::base::
                         substitution_succeeds<multiply_result_t, double, quantity<simple_unit, std::uint32_t>>);

  using qtyd = quantity<simple_unit, double>;

  constexpr double val1 = 1.23;
  constexpr double val2 = -99.23;
  constexpr std::uint32_t val3 = 42;

  constexpr qtyd qty1{val1};
  constexpr qtyd qty2 = qty1 * val2;
  constexpr qtyd qty3 = val3 * qty2;

  ASSERT_NEAR(qty2.count_of<simple_unit>(), val1 * val2, 1e-300);
  ASSERT_NEAR(qty3.count_of<simple_unit>(), val1 * val2 * val3, 1e-300);
}

class multiplyable_unit {};
class other_multiplyable_unit {};
class product_unit {};
}  // namespace
template <>
struct arene::base::units_combination_traits<multiplyable_unit, other_multiplyable_unit> {
  using product_type = product_unit;
};

namespace {

/// @test Two `quantity` objects can be multiplied if the combination traits specify the result. The result is a
/// `quantity` of the specified type, holding the value resulting from multiplying the two values together.
TEST(Quantity, CanMultiplyByOtherUnitsIfTraitsDefined) {
  using qty1 = quantity<multiplyable_unit, double>;
  using qty2 = quantity<other_multiplyable_unit, double>;
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<multiply_result_t, qty1, qty2>);
  STATIC_ASSERT_TRUE(std::is_same<multiply_result_t<qty1, qty2>, quantity<product_unit, double>>::value);

  constexpr double value1 = 1.23;
  constexpr double value2 = 42.98;
  constexpr qty1 lhs{value1};
  constexpr qty2 rhs{value2};

  constexpr auto product = lhs * rhs;

  ASSERT_NEAR(product.count_of<product_unit>(), value1 * value2, 1e-300);
}

/// @test Two `quantity` objects can be multiplied if the combination traits specify the result, the representation is
/// unsigned and the result is in range. The result is a `quantity` of the specified type, holding the value resulting
/// from multiplying the two values together.
TEST(Quantity, CanMultiplyByOtherUnitsIfTraitsDefinedAndValuesInRangeForUnsigned) {
  using qty1 = quantity<multiplyable_unit, std::uint32_t>;
  using qty2 = quantity<other_multiplyable_unit, std::uint32_t>;
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<multiply_result_t, qty1, qty2>);
  STATIC_ASSERT_TRUE(std::is_same<multiply_result_t<qty1, qty2>, quantity<product_unit, std::uint32_t>>::value);

  constexpr std::uint32_t value1 = 10;
  constexpr std::uint32_t value2 = 42;
  constexpr qty1 lhs{value1};
  constexpr qty2 rhs{value2};

  STATIC_ASSERT_EQ((lhs * rhs).count_of<product_unit>(), value1 * value2);
}

/// @test Two `quantity` objects can be multiplied if the combination traits specify the result, the representation is
/// signed and the result is in range. The result is a `quantity` of the specified type, holding the value resulting
/// from multiplying the two values together.
TEST(Quantity, CanMultiplyByOtherUnitsIfTraitsDefinedAndValuesInRangeForSigned) {
  using qty1 = quantity<multiplyable_unit, std::int8_t>;
  using qty2 = quantity<other_multiplyable_unit, std::int8_t>;
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<multiply_result_t, qty1, qty2>);
  STATIC_ASSERT_TRUE(std::is_same<multiply_result_t<qty1, qty2>, quantity<product_unit, std::int8_t>>::value);

  constexpr std::int8_t value1 = 10;
  constexpr std::int8_t value2 = -1;
  constexpr qty1 lhs{value1};
  constexpr qty2 rhs{value2};

  STATIC_ASSERT_EQ((lhs * rhs).count_of<product_unit>(), value1 * value2);
}

/// @test A `quantity` object with a signed representation can only be multiplied by a `quantity` with an unsigned
/// representation if the "common type" of the representations is signed.
TEST(Quantity, CanOnlyMultiplySignedByUnsignedIfBiggerResult) {
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<
                     multiply_result_t,
                     quantity<multiplyable_unit, std::int8_t>,
                     quantity<other_multiplyable_unit, std::uint8_t>>);
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<
                     multiply_result_t,
                     quantity<multiplyable_unit, std::int16_t>,
                     quantity<other_multiplyable_unit, std::uint16_t>>);
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<
                      multiply_result_t,
                      quantity<multiplyable_unit, std::int32_t>,
                      quantity<other_multiplyable_unit, std::uint32_t>>);
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<
                      multiply_result_t,
                      quantity<multiplyable_unit, std::int64_t>,
                      quantity<other_multiplyable_unit, std::uint64_t>>);
}

/// @test Two `quantity` objects cannot be divided if the combination traits do not specify a result
TEST(Quantity, CannotDivideUnitsNotEnabled) {
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<
                      divide_result_t,
                      quantity<simple_unit, double>,
                      quantity<simple_unit, double>>);
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<
                      divide_result_t,
                      quantity<point_unit, double>,
                      quantity<point_unit, double>>);
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<
                      divide_result_t,
                      quantity<simple_unit, double>,
                      quantity<point_unit, double>>);
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<
                      divide_result_t,
                      quantity<foo_unit, double>,
                      quantity<foo_unit, double>>);
}

/// @test A `quantity` can be divided by a scalar, yielding a result of the same `quantity` type, but with the value
/// divided by the scalar
TEST(Quantity, CanDivideByScalarIfResultInRange) {
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<divide_result_t, quantity<simple_unit, double>, double>);
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<divide_result_t, quantity<simple_unit, double>, std::uint32_t>);
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<divide_result_t, quantity<simple_unit, std::uint32_t>, double>);
  STATIC_ASSERT_FALSE(arene::base::substitution_succeeds<divide_result_t, double, quantity<simple_unit, double>>);
  STATIC_ASSERT_FALSE(arene::base::
                          substitution_succeeds<divide_result_t, std::uint32_t, quantity<simple_unit, double>>);
  STATIC_ASSERT_FALSE(arene::base::
                          substitution_succeeds<divide_result_t, double, quantity<simple_unit, std::uint32_t>>);
  STATIC_ASSERT_FALSE(arene::base::
                          substitution_succeeds<divide_result_t, quantity<simple_unit, std::uint32_t>, std::uint32_t>);
  STATIC_ASSERT_FALSE(arene::base::
                          substitution_succeeds<divide_result_t, std::uint32_t, quantity<simple_unit, std::uint32_t>>);

  using qtyd = quantity<simple_unit, double>;

  constexpr double val1 = 1.23;
  constexpr double val2 = -99.23;
  constexpr std::uint32_t val3 = 42;

  constexpr qtyd qty1{val1};
  constexpr qtyd qty2 = qty1 / val2;
  constexpr qtyd qty3 = qty2 / val3;

  ASSERT_NEAR(qty2.count_of<simple_unit>(), val1 / val2, 1e-300);
  ASSERT_NEAR(qty3.count_of<simple_unit>(), val1 / val2 / val3, 1e-300);
}

class divideable_unit {};
class other_divideable_unit {};
class ratio_unit {};
}  // namespace
template <>
struct arene::base::units_combination_traits<divideable_unit, other_divideable_unit> {
  using ratio_type = ratio_unit;
};

namespace {

/// @test A `quantity` can be divided by another `quantity` if the combination traits define the resulting units; the
/// result is a `quantity` with the specified unit type, and a value which is the ratio of the two values
TEST(Quantity, CanDivideByOtherUnitsIfTraitsDefined) {
  using qty1 = quantity<divideable_unit, double>;
  using qty2 = quantity<other_divideable_unit, double>;
  STATIC_ASSERT_TRUE(arene::base::substitution_succeeds<divide_result_t, qty1, qty2>);
  STATIC_ASSERT_TRUE(std::is_same<divide_result_t<qty1, qty2>, quantity<ratio_unit, double>>::value);

  constexpr double value1 = 1.23;
  constexpr double value2 = 42.98;
  constexpr qty1 lhs{value1};
  constexpr qty2 rhs{value2};

  constexpr auto ratio = lhs / rhs;

  ASSERT_NEAR(ratio.count_of<ratio_unit>(), value1 / value2, 1e-300);
}

/// @test The representation type of `quantity` is `double` if not specified.
TEST(Quantity, DefaultRepIsDouble) {
  STATIC_ASSERT_TRUE(std::is_same<quantity<simple_unit>, quantity<simple_unit, double>>::value);
  STATIC_ASSERT_TRUE(std::is_same<quantity<point_unit>, quantity<point_unit, double>>::value);
}
}  // namespace
