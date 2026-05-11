// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::units_detail::explicit_base_set"
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_EXPLICIT_BASE_SET_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_EXPLICIT_BASE_SET_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/remove_const.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/type_traits/always_false.hpp"
#include "arene/base/units/base_kind_set.hpp"
#include "arene/base/units/is_explicit.hpp"
#include "arene/base/units/is_quantity_kind.hpp"
#include "arene/base/units/kind_with_exponent_fwd.hpp"
#include "arene/base/units/quantity_kind.hpp"
#include "arene/base/units/scaled_unit.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-A3_1_5-a "False positive: all functions are constexpr so are intended to be inlined"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

namespace arene {
namespace base {

namespace combine_kinds_detail {
/// @brief A template that represents a quantity kind that is a combination of base quantity kinds
/// @tparam BaseKinds The base quantity kinds to combine
template <typename... BaseKinds>
class combined_quantity_kind;
}  // namespace combine_kinds_detail

namespace units_detail {

/// @brief provide a barrier type to treat a quantity kind as explicit
/// @tparam Kind quantity kind type
///
/// Creates a quantity kind barrier, blocking rules for implicit kinds when
/// determining the explicit base set.
template <typename Kind>
class explicit_base_barrier : public quantity_kind<explicit_base_barrier<Kind>> {};

namespace explicit_base_set_detail {

/// @brief helper used to denote that the explicit base set cannot be determined
/// for the specified type
/// @tparam Kind quantity kind of which to obtain the explicit base set
/// @return never actually returns as use of this function is always a
///   compilation error.
template <typename Kind>
constexpr auto explicit_base_set_error() -> std::nullptr_t {
  static_assert(  //
      is_quantity_kind_v<Kind>,
      "explicit base set cannot be determined for invalid quantity kind"
  );
  static_assert(
      always_false_v<Kind>,
      "internal error: a valid quantity kind was sorted to the explicit_base_set_error function by mistake"
  );
  return {};
}

/// @brief obtains the explicit base set of a quantity kind
/// @tparam Kind quantity kind of which to obtain the explicit base set
///
/// This primary template is always an error.
template <typename Kind, typename = constraints<>>
extern constexpr auto explicit_base_set = explicit_base_set_error<Kind>();

/// @brief obtains the explicit base set of a quantity kind
/// @tparam Kind quantity kind of which to obtain the explicit base set
///
/// This specialization handles an explicit quantity kind.
template <typename Kind>
extern constexpr auto explicit_base_set<Kind, constraints<std::enable_if_t<is_explicit_v<Kind>>>> =
    base_kind_set<Kind>{base_kind_exponents{1U, 0U}};

// scaled unit not yet handled

/// @brief obtains the explicit base set of a quantity kind
/// @tparam Kind quantity kind of which to obtain the explicit base set
///
/// This specialization handles an quantity kind with exponent.
template <typename Kind, exponent_t PositiveExponent, exponent_t NegativeExponent>
extern constexpr auto
    explicit_base_set<quantity_kind_with_exponent<Kind, PositiveExponent, NegativeExponent, constraints<>>> =  //
    set_union(                                                                                                 //
        PositiveExponent * explicit_base_set<typename Kind::quantity_kind_type>,                               //
        -NegativeExponent* explicit_base_set<typename Kind::quantity_kind_type>                                //
    );

/// @brief obtains the explicit base set of a quantity kind
/// @tparam Kind quantity kind of which to obtain the explicit base set
///
/// This specialization handles a combined quantity kind.
template <typename... BaseKinds>
extern constexpr auto explicit_base_set<combine_kinds_detail::combined_quantity_kind<BaseKinds...>> =
    set_union(explicit_base_set<BaseKinds>...);

/// @brief obtains the explicit base set of a quantity kind
/// @tparam BaseUnit the base unit for the scaled unit
/// @tparam Scale the scale factor for the scaled unit
///
/// This specialization handles a scaled quantity kind.
template <typename BaseUnit, typename Scale>
extern constexpr auto explicit_base_set<scaled_unit_detail::scaled_unit_impl<BaseUnit, Scale>> =
    explicit_base_set<BaseUnit>;

}  // namespace explicit_base_set_detail

/// @brief obtains the explicit base set of a quantity kind
/// @tparam Kind quantity kind of which to obtain the explicit base set
///
/// Obtains a set of elements specifying the explicit base set of a quantity
/// kind. Each set element is a triple of <c>(Kind, PositiveExponent,
/// NegativeExponent)</c>.
///
/// * If @c Kind is an explicit quantity kind, this set is <c>{(Kind, 1,
///   0)}</c>.
/// * otherwise, if @c Kind is a scaled unit, this set is the explict base set
///   for the base unit of @c Kind.
/// * otherwise, if @c Kind is a composite quantity kind consisting of @c I
///   quantity kinds @c Ki with explicit base sets @c E(K_ij) equal to <c>{(K_ij,
///   P_ij, N_ij)}</c>, then the explicit base set of @c Kind is the set <c>{(K_n,
///   P_n, N_n)}</c> where the quantity kinds @c K_n are the distinct quantity
///   kinds from @c K_ij, and @c P_n is the sum of the corresponding positive
///   exponents <c>P_i * P_ij + N_i * N_ij</c>, @c N_n is the sum of the
///   corresponding negative exponents <c>N_i * P_ij + P_i * N_ij</c>, where
///   @c P_i and @c N_i are the positive and negative exponents of @c K_i
/// * otherwise, there is an error.
///
template <typename Kind>
extern constexpr auto explicit_base_set = explicit_base_set_detail::explicit_base_set<std::remove_cv_t<Kind>>;

/// @brief trait to obtain the type of the explicit base set of a quantity kind
/// @tparam Kind quantity kind of which to obtain the explicit base set
template <typename Kind>
using explicit_base_set_t = std::remove_const_t<decltype(explicit_base_set<Kind>)>;

}  // namespace units_detail

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_EXPLICIT_BASE_SET_HPP_
