// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_KIND_WITH_EXPONENT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_KIND_WITH_EXPONENT_HPP_

#include "arene/base/constraints/constraints.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/units/has_explicit_units.hpp"
#include "arene/base/units/has_parent_kind.hpp"
#include "arene/base/units/is_unit.hpp"
#include "arene/base/units/kind_with_exponent_fwd.hpp"  // IWYU pragma: keep
#include "arene/base/units/quantity_kind.hpp"
#include "arene/base/units/quantity_unit.hpp"
#include "arene/base/units/selected_unit_for.hpp"

namespace arene {
namespace base {

namespace units_detail {

/// @brief Helper class to determine the result of raising the specified quantity kind to the specified positive and
/// negative exponents. Both @c PositiveExponent and @c NegativeExponent must be non-negative. The effective exponent is
/// @c PositiveExponent - @c NegativeExponent however they are stored separately to allow distinguishing @c m/m from @c
/// s/s or @c m^2/m^2
/// @tparam Kind The quantity kind to raise to an exponent
/// @tparam PositiveExponent The positive exponent to raise it to
/// @tparam NegativeExponent The negative exponent to raise it to
template <
    typename Kind,
    exponent_t PositiveExponent,
    exponent_t NegativeExponent,
    typename /* constraints, defaulted on forward declaration */>
class kind_with_exponent_helper {
  static_assert(PositiveExponent >= 0, "The exponents must be non-negative");
  static_assert(NegativeExponent >= 0, "The exponents must be non-negative");

 public:
  /// @brief An alias for a quantity kind representing @c Kind raised to the positive and negative exponents
  using type = quantity_kind_with_exponent<Kind, PositiveExponent, NegativeExponent>;
};

// parasoft-begin-suppress AUTOSAR-A14_7_2-a "False positive: The primary template is defined in this file"
/// @brief Helper class to determine the result of raising the specified quantity kind to the specified positive and
/// negative exponents. This specialization is for when the positive exponent is 1 and the negative exponent is zero
/// @tparam Kind The quantity kind to raise to an exponent of 1
template <typename Kind>
class kind_with_exponent_helper<Kind, 1, 0> {
 public:
  /// @brief An alias for @c Kind
  using type = Kind;
};
// parasoft-end-suppress AUTOSAR-A14_7_2-a

// parasoft-begin-suppress AUTOSAR-A14_7_2-a "The primary template is forward declared to avoid circular dependencies"
/// @brief Implementation class for a quantity kind raised to the specified positive and
/// negative exponents. Both @c PositiveExponent and @c NegativeExponent must be non-negative. The effective exponent is
/// @c PositiveExponent - @c NegativeExponent however they are stored separately to allow distinguishing @c m/m from @c
/// s/s or @c m^2/m^2
/// @tparam Kind The quantity kind to raise to an exponent
/// @tparam PositiveExponent The positive exponent to raise it to
/// @tparam NegativeExponent The negative exponent to raise it to
template <typename Kind, exponent_t PositiveExponent, exponent_t NegativeExponent>
class quantity_kind_with_exponent<
    Kind,
    PositiveExponent,
    NegativeExponent,
    constraints<std::enable_if_t<!has_parent_kind_v<Kind>>, std::enable_if_t<!has_explicit_units_v<Kind>>>>
    : public quantity_kind<quantity_kind_with_exponent<Kind, PositiveExponent, NegativeExponent>> {
 public:
  /// @brief Denotes that quantity kind is implicit and does not mark a
  /// boundary when aggregating and simplifying quantity kind types.
  using is_implicit = void;
};
// parasoft-end-suppress AUTOSAR-A14_7_2-a

// parasoft-begin-suppress AUTOSAR-A14_7_2-a "The primary template is forward declared to avoid circular dependencies"
/// @brief Implementation class for a quantity kind raised to the specified positive and
/// negative exponents. Both @c PositiveExponent and @c NegativeExponent must be non-negative. The effective exponent is
/// @c PositiveExponent - @c NegativeExponent however they are stored separately to allow distinguishing @c m/m from @c
/// s/s or @c m^2/m^2
/// @tparam Kind The quantity kind to raise to an exponent
/// @tparam PositiveExponent The positive exponent to raise it to
/// @tparam NegativeExponent The negative exponent to raise it to
template <typename Kind, exponent_t PositiveExponent, exponent_t NegativeExponent>
class quantity_kind_with_exponent<
    Kind,
    PositiveExponent,
    NegativeExponent,
    constraints<std::enable_if_t<is_unit_v<Kind>>>>
    : public quantity_unit<
          quantity_kind_with_exponent<Kind, PositiveExponent, NegativeExponent>,
          typename kind_with_exponent_helper<
              typename Kind::parent_quantity_kind_type,
              PositiveExponent,
              NegativeExponent>::type> {
 public:
  /// @brief Denotes that quantity kind is implicit and does not mark a
  /// boundary when aggregating and simplifying quantity kind types.
  using is_implicit = void;
};
// parasoft-end-suppress AUTOSAR-A14_7_2-a

// parasoft-begin-suppress AUTOSAR-A14_7_2-a "The primary template is forward declared to avoid circular dependencies"
/// @brief Implementation class for a quantity kind raised to the specified positive and
/// negative exponents. Both @c PositiveExponent and @c NegativeExponent must be non-negative. The effective exponent is
/// @c PositiveExponent - @c NegativeExponent however they are stored separately to allow distinguishing @c m/m from @c
/// s/s or @c m^2/m^2
/// @tparam Kind The quantity kind to raise to an exponent
/// @tparam PositiveExponent The positive exponent to raise it to
/// @tparam NegativeExponent The negative exponent to raise it to
template <typename Kind, exponent_t PositiveExponent, exponent_t NegativeExponent>
class quantity_kind_with_exponent<
    Kind,
    PositiveExponent,
    NegativeExponent,
    constraints<
        std::enable_if_t<!is_unit_v<Kind>>,
        std::enable_if_t<has_parent_kind_v<Kind>>,
        std::enable_if_t<!has_explicit_units_v<Kind>>>>
    : public quantity_kind<
          quantity_kind_with_exponent<Kind, PositiveExponent, NegativeExponent>,
          typename kind_with_exponent_helper<
              typename Kind::parent_quantity_kind_type,
              PositiveExponent,
              NegativeExponent>::type> {
 public:
  /// @brief Denotes that quantity kind is implicit and does not mark a
  /// boundary when aggregating and simplifying quantity kind types.
  using is_implicit = void;
};
// parasoft-end-suppress AUTOSAR-A14_7_2-a

// parasoft-begin-suppress AUTOSAR-A14_7_2-a "The primary template is forward declared to avoid circular dependencies"
/// @brief Implementation class for a quantity kind raised to the specified positive and
/// negative exponents. Both @c PositiveExponent and @c NegativeExponent must be non-negative. The effective exponent is
/// @c PositiveExponent - @c NegativeExponent however they are stored separately to allow distinguishing @c m/m from @c
/// s/s or @c m^2/m^2
/// @tparam Kind The quantity kind to raise to an exponent
/// @tparam PositiveExponent The positive exponent to raise it to
/// @tparam NegativeExponent The negative exponent to raise it to
template <typename Kind, exponent_t PositiveExponent, exponent_t NegativeExponent>
class quantity_kind_with_exponent<
    Kind,
    PositiveExponent,
    NegativeExponent,
    constraints<std::enable_if_t<!has_parent_kind_v<Kind>>, std::enable_if_t<has_explicit_units_v<Kind>>>>
    : public quantity_kind<
          quantity_kind_with_exponent<Kind, PositiveExponent, NegativeExponent>,
          typename kind_with_exponent_helper<selected_unit_for_t<Kind>, PositiveExponent, NegativeExponent>::type> {
 public:
  /// @brief Denotes that quantity kind is implicit and does not mark a
  /// boundary when aggregating and simplifying quantity kind types.
  using is_implicit = void;
};
// parasoft-end-suppress AUTOSAR-A14_7_2-a

// parasoft-begin-suppress AUTOSAR-A14_7_2-a "False positive: The primary template is defined in this file"
/// @brief Implementation class for a quantity kind raised to the specified positive and
/// negative exponents. Both @c PositiveExponent and @c NegativeExponent must be non-negative. The effective exponent is
/// @c PositiveExponent - @c NegativeExponent however they are stored separately to allow distinguishing @c m/m from @c
/// s/s or @c m^2/m^2
/// @tparam Kind The quantity kind to raise to an exponent
/// @tparam PositiveExponent The positive exponent to raise it to
/// @tparam NegativeExponent The negative exponent to raise it to
template <typename Kind, exponent_t PositiveExponent, exponent_t NegativeExponent>
class quantity_kind_with_exponent<
    Kind,
    PositiveExponent,
    NegativeExponent,
    constraints<std::enable_if_t<has_parent_kind_v<Kind>>, std::enable_if_t<has_explicit_units_v<Kind>>>>
    : public quantity_kind<
          quantity_kind_with_exponent<Kind, PositiveExponent, NegativeExponent>,
          typename kind_with_exponent_helper<
              typename Kind::parent_quantity_kind_type,
              PositiveExponent,
              NegativeExponent>::type,
          typename kind_with_exponent_helper<selected_unit_for_t<Kind>, PositiveExponent, NegativeExponent>::type> {
 public:
  /// @brief Denotes that quantity kind is implicit and does not mark a
  /// boundary when aggregating and simplifying quantity kind types.
  using is_implicit = void;
};
// parasoft-end-suppress AUTOSAR-A14_7_2-a

// parasoft-begin-suppress AUTOSAR-A14_7_2-a "The template is forward declared to avoid circular dependencies"
/// @brief Helper class to determine the result of raising the specified quantity kind to the specified exponent
/// @tparam BaseKind The quantity kind to raise to an exponent
/// @tparam BasePositiveExponent The positive exponent to it is already raised to
/// @tparam BaseNegativeExponent The negative exponent to it is already raised to
/// @tparam PositiveExponent The positive exponent to raise it to
/// @tparam NegativeExponent The negative exponent to raise it to
template <
    typename BaseKind,
    exponent_t BasePositiveExponent,
    exponent_t BaseNegativeExponent,
    exponent_t PositiveExponent,
    exponent_t NegativeExponent>
class kind_with_exponent_helper<
    quantity_kind_with_exponent<BaseKind, BasePositiveExponent, BaseNegativeExponent>,
    PositiveExponent,
    NegativeExponent,
    constraints<
        // negated expr matches constraint for other specialization
        // NOLINTNEXTLINE(readability-simplify-boolean-expr)
        std::enable_if_t<!(PositiveExponent == 1 && NegativeExponent == 0)>>> {
 public:
  /// @brief An alias for a quantity kind representing the result of raising the specified quantity kind to the
  /// specified exponent
  using type = typename kind_with_exponent_helper<
      BaseKind,
      BasePositiveExponent * PositiveExponent + BaseNegativeExponent * NegativeExponent,
      BasePositiveExponent * NegativeExponent + BaseNegativeExponent * PositiveExponent>::type;
};
// parasoft-end-suppress AUTOSAR-A14_7_2-a

}  // namespace units_detail

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_KIND_WITH_EXPONENT_HPP_
