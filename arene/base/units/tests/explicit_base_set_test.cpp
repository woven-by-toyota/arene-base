// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gtest/gtest.h>

#include "arene/base/algorithm/all_of.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/ratio.hpp"
#include "arene/base/units.hpp"
#include "arene/base/units/kind_with_exponent_fwd.hpp"

#if ARENE_IS_OFF(ARENE_STDLIB_LIBARENECXX)
#include <iostream>

namespace arene {
namespace base {
namespace units_detail {

// uses canonical variable names
// NOLINTBEGIN(readability-identifier-length)

template <class... Kinds>
auto operator<<(std::ostream& os, base_kind_set<Kinds...> const& bases) -> std::ostream& {
  constexpr auto kind_names =
      arene::base::array<arene::base::string_view, sizeof...(Kinds)>{arene::base::type_name_v<Kinds>...};

  os << "\n{\n";
  auto i = std::size_t{};
  for (auto const& exp : bases.exponents) {
    os << "  ( " << kind_names[i++] << ", " << exp.positive() << ", " << exp.negative() << " )\n";
  }
  os << "}\n";
  return os;
}

// NOLINTEND(readability-identifier-length)

}  // namespace units_detail
}  // namespace base
}  // namespace arene
#endif

namespace {

namespace test {

/// @brief expected positive and negative elements of a quantity kind
/// @tparam Kind quantity kind
/// @tparam PositiveExponent positive exponent value
/// @tparam NegativeExponent negative exponent value
///
/// Helper used in tests to specify the expected exponent values for a quantity
/// kind in the explicit base set.
template <class Kind, std::size_t PositiveExponent, std::size_t NegativeExponent>
struct element {
  using kind_type = Kind;
  static constexpr auto positive_exponent = PositiveExponent;
  static constexpr auto negative_exponent = NegativeExponent;
};

/// @brief expected explicit base set
/// @tparam Elements expected elements in the set
///
/// Helper used in tests to specify the expected elements in the explicit base
/// set of a quantity kind.
template <class... Elements>
struct set_t {
  /// @brief alias of the implementation detail type
  template <class... Kinds>
  using explicit_bases = arene::base::units_detail::base_kind_set<Kinds...>;

  /// @brief check if the provided quantity kind types are the same as kinds in
  /// `Elements...`
  /// @tparam Kinds quantity kinds to compare with
  template <class... Kinds>
  static constexpr auto has_same_kinds_v = std::is_same<
      // `combine_kinds` is arbitrarily used to order the types in the list
      arene::base::combine_kinds_t<Kinds...>,
      arene::base::combine_kinds_t<typename Elements::kind_type...>>::value;

  /// @brief check if the explicit base set is equal to the expected set
  ///
  /// This overload is selected if the quantity kind types match.
  ///
  /// @return `true` if for quantity kind types, the positive exponent value in
  /// `explicit_bases` equals the positive exponent value in `set_t` and the
  /// negative exponent value in `explicit_bases` equals the negative exponent
  /// value in `set_t`. Otherwise, `false`.
  template <  //
      class... Kinds,
      arene::base::constraints<std::enable_if_t<has_same_kinds_v<Kinds...>>> = nullptr>
  friend auto operator==(explicit_bases<Kinds...> const& bases, set_t) -> bool {
    return arene::base::all_of(
        {((bases.template exponent_for<typename Elements::kind_type>().positive() == Elements::positive_exponent) &&
          (bases.template exponent_for<typename Elements::kind_type>().negative() == Elements::negative_exponent))...}
    );
  }

  /// @brief check if the explicit base set is equal to the expected set
  ///
  /// This overload is selected if the quantity kind types do not match and
  /// always returns `false`.
  template <  //
      class... Kinds,
      arene::base::constraints<std::enable_if_t<!has_same_kinds_v<Kinds...>>> = nullptr>
  friend auto operator==(explicit_bases<Kinds...> const&, set_t) -> std::false_type {
    return {};
  }
};

/// @brief specifies a constant value of the expected explicit base set
/// @tparam Elements expected elements in the set
template <class... Elements>
constexpr auto set = set_t<Elements...>{};

}  // namespace test

class length : public arene::base::quantity_kind<length> {};
class time : public arene::base::quantity_kind<time> {};

/// @test the explicit base set for an explicit quantity kind contains a single element with the kind, positive exponent
/// of 1, and negative exponent of 0
TEST(ExplicitBaseSet, ExplicitQuantityKind) {
  class reach : public arene::base::quantity_kind<reach, length> {};
  class area : public arene::base::quantity_kind<area, arene::base::kind_with_exponent_t<length, 2>> {};
  class volume : public arene::base::quantity_kind<volume, arene::base::combine_kinds_t<area, length>> {};

  ASSERT_EQ((arene::base::units_detail::explicit_base_set<length>), (test::set<test::element<length, 1, 0>>));
  ASSERT_EQ((arene::base::units_detail::explicit_base_set<time>), (test::set<test::element<time, 1, 0>>));
  ASSERT_EQ((arene::base::units_detail::explicit_base_set<reach>), (test::set<test::element<reach, 1, 0>>));
  ASSERT_EQ((arene::base::units_detail::explicit_base_set<area>), (test::set<test::element<area, 1, 0>>));
  ASSERT_EQ((arene::base::units_detail::explicit_base_set<volume>), (test::set<test::element<volume, 1, 0>>));
}

/// @test the explicit base set for a quantity kind ignores cv-qual of the kind type
TEST(ExplicitBaseSet, IgnoresKindCvQual) {
  ASSERT_EQ(                                                         //
      (arene::base::units_detail::explicit_base_set<length const>),  //
      (test::set<test::element<length, 1, 0>>)
  );
  ASSERT_EQ(                                                            //
      (arene::base::units_detail::explicit_base_set<length volatile>),  //
      (test::set<test::element<length, 1, 0>>)
  );
  ASSERT_EQ(                                                                  //
      (arene::base::units_detail::explicit_base_set<length const volatile>),  //
      (test::set<test::element<length, 1, 0>>)
  );
}

/// @test the explicit base set for an explicit quantity kind raised to an exponent contains a single element with the
/// kind and exponent values depending on the exponent number
TEST(ExplicitBaseSet, ExplicitQuantityKindRaisedToAnExponent) {
  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<arene::base::kind_with_exponent_t<length, 1>>),
      (test::set<test::element<length, 1, 0>>)
  );

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<arene::base::kind_with_exponent_t<length, 2>>),
      (test::set<test::element<length, 2, 0>>)
  );

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<arene::base::kind_with_exponent_t<length, 3>>),
      (test::set<test::element<length, 3, 0>>)
  );

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<arene::base::kind_with_exponent_t<length, 0>>),
      (test::set<test::element<length, 0, 0>>)
  );

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<arene::base::kind_with_exponent_t<length, -1>>),
      (test::set<test::element<length, 0, 1>>)
  );

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<arene::base::kind_with_exponent_t<length, -2>>),
      (test::set<test::element<length, 0, 2>>)
  );
}

/// @test the explicit base set for a scaled quantity unit is just that unit
TEST(ExplicitBaseSet, BaseSetForScaledUnitIsTheUnderlyingUnit) {
  class length_unit : public arene::base::quantity_unit<length_unit, length> {};

  using scaled = arene::base::scaled_unit<length_unit, std::kilo>;

  ASSERT_EQ((arene::base::units_detail::explicit_base_set<scaled>), (test::set<test::element<length_unit, 1, 0>>));

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<arene::base::kind_with_exponent_t<scaled, 2>>),
      (test::set<test::element<length_unit, 2, 0>>)
  );

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<arene::base::kind_with_exponent_t<scaled, 3>>),
      (test::set<test::element<length_unit, 3, 0>>)
  );

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<arene::base::kind_with_exponent_t<scaled, 0>>),
      (test::set<test::element<length_unit, 0, 0>>)
  );

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<arene::base::kind_with_exponent_t<scaled, -1>>),
      (test::set<test::element<length_unit, 0, 1>>)
  );

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<arene::base::kind_with_exponent_t<scaled, -2>>),
      (test::set<test::element<length_unit, 0, 2>>)
  );
}

/// @test the explicit base set of a combined quantity kind containing scaled units decays the scaled unit
TEST(ExplicitBaseSet, ScaledUnitsInCombinedQuantities) {
  class length_unit : public arene::base::quantity_unit<length_unit, length> {};
  using scaled = arene::base::scaled_unit<length_unit, std::kilo>;

  using arene::base::combine_kinds_t;
  using arene::base::units_detail::explicit_base_set;

  ASSERT_EQ(  //
      (explicit_base_set<combine_kinds_t<scaled>>),
      (test::set<test::element<length_unit, 1, 0>>)
  );

  ASSERT_EQ(  //
      (explicit_base_set<combine_kinds_t<scaled, scaled>>),
      (test::set<test::element<length_unit, 2, 0>>)
  );

  ASSERT_EQ(  //
      (explicit_base_set<combine_kinds_t<scaled, length_unit>>),
      (test::set<test::element<length_unit, 2, 0>>)
  );

  ASSERT_EQ(  //
      (explicit_base_set<combine_kinds_t<scaled, length_unit::with_exponent<-2>>>),
      (test::set<test::element<length_unit, 1, 2>>)
  );
}

/// @test the explicit base set for a combined quantity kind contains all the explicit quantity kind bases with the
/// aggregate exponents
TEST(ExplicitBaseSet, CombinedQuantityKind) {
  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<  //
          arene::base::combine_kinds_t<               //
              length>>),                              //
      (test::set<                                     //
          test::element<length, 1, 0>>)
  );
  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<  //
          arene::base::combine_kinds_t<               //
              length,
              length>>),  //
      (test::set<         //
          test::element<length, 2, 0>>)
  );
  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<  //
          arene::base::combine_kinds_t<               //
              length,
              arene::base::kind_with_exponent_t<length, -1>>>),  //
      (test::set<                                                //
          test::element<length, 1, 1>>)
  );

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<  //
          arene::base::combine_kinds_t<               //
              length,
              time>>),  //
      (test::set<       //
          test::element<length, 1, 0>,
          test::element<time, 1, 0>>)
  );
  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<  //
          arene::base::combine_kinds_t<               //
              length,
              arene::base::kind_with_exponent_t<time, -1>>>),  //
      (test::set<                                              //
          test::element<length, 1, 0>,
          test::element<time, 0, 1>>)
  );
  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<  //
          arene::base::combine_kinds_t<
              length,
              arene::base::kind_with_exponent_t<time, -1>,
              arene::base::kind_with_exponent_t<time, -2>>>),  //
      (test::set<                                              //
          test::element<length, 1, 0>,
          test::element<time, 0, 3>>)
  );
}

/// @test the explicit base set for a combined quantity kind raised to an exponent contains all the explicit quantity
/// kind bases with the aggregate exponents
TEST(ExplicitBaseSet, CombinedQuantityKindRaisedToExponent) {
  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<  //
          arene::base::kind_with_exponent_t<          //
              arene::base::combine_kinds_t<
                  length,
                  arene::base::kind_with_exponent_t<time, -1>,
                  arene::base::kind_with_exponent_t<time, -1>>,
              0>>

      ),
      (test::set<  //
          test::element<length, 0, 0>,
          test::element<time, 0, 0>>)
  );

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<  //
          arene::base::kind_with_exponent_t<          //
              arene::base::combine_kinds_t<
                  length,
                  arene::base::kind_with_exponent_t<time, -1>,
                  arene::base::kind_with_exponent_t<time, -1>>,
              1>>

      ),
      (test::set<  //
          test::element<length, 1, 0>,
          test::element<time, 0, 2>>)
  );

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<  //
          arene::base::kind_with_exponent_t<          //
              arene::base::combine_kinds_t<
                  length,
                  arene::base::kind_with_exponent_t<time, -1>,
                  arene::base::kind_with_exponent_t<time, -1>>,
              2>>

      ),
      (test::set<  //
          test::element<length, 2, 0>,
          test::element<time, 0, 4>>)
  );

  ASSERT_EQ(
      (arene::base::units_detail::explicit_base_set<  //
          arene::base::kind_with_exponent_t<          //
              arene::base::combine_kinds_t<
                  length,
                  arene::base::kind_with_exponent_t<time, -1>,
                  arene::base::kind_with_exponent_t<time, -1>>,
              -2>>

      ),
      (test::set<  //
          test::element<length, 0, 2>,
          test::element<time, 4, 0>>)
  );
}

}  // namespace
