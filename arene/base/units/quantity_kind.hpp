// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_QUANTITY_KIND_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_QUANTITY_KIND_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_class.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/units/detail/missing.hpp"
#include "arene/base/units/is_possible_unit_for.hpp"
#include "arene/base/units/is_quantity_kind.hpp"
#include "arene/base/units/is_unit.hpp"
#include "arene/base/units/kind_with_exponent_fwd.hpp"
#include "arene/base/units/quantity_origin.hpp"

namespace arene {
namespace base {

namespace quantity_kind_detail {

/// @brief Helper to check if @c Type is @c Missing
template <typename Type>
extern constexpr bool is_missing_v = std::is_same<Type, units_detail::missing>::value;

/// @brief helper used to rebind the unit type of a quantity kind
template <typename>
auto rebind_quantity_kind_unit_type() -> void = delete;

/// @brief Mixin base class for declaring the unit of a quantity kind
/// @tparam Unit the type of the default unit for the quantity kind
template <typename Unit>
class quantity_kind_unit_mixin {
 public:
  /// @brief Type alias to specify the unit for this quantity kind
  using unit_type = Unit;
};

/// @brief Mixin base class for declaring the unit of a quantity kind
template <>
class quantity_kind_unit_mixin<units_detail::missing> {};

/// @brief Mixin base class for declaring the origin of a quantity kind
/// @tparam Origin the type of the origin for the quantity kind
template <typename Origin>
class quantity_kind_origin_mixin {
 public:
  /// @brief Type alias to specify the origin for this quantity kind
  using origin = Origin;
};

/// @brief Mixin base class for declaring the origin of a quantity kind
template <>
class quantity_kind_origin_mixin<units_detail::missing> {};

/// @brief Mixin base class for declaring a parent quantity kind for a derived quantity kind
/// @tparam ParentKind the type of the parent quantity kind for the derived quantity kind
template <typename ParentKind>
class quantity_kind_parent_mixin {
 public:
  /// @brief Type alias for the parent quantity kind
  using parent_quantity_kind_type = ParentKind;
};

/// @brief Mixin base class for declaring a parent quantity kind for a derived quantity kind
/// @tparam ParentKind the type of the parent quantity kind for the derived quantity kind
template <>
class quantity_kind_parent_mixin<units_detail::missing> {};

/// @brief Base class for declaring a quantity kind
/// @tparam Derived the derived class which represents the quantity kind
/// @tparam OptionalParentKind the type of the parent quantity kind for @c Derived if there is one, otherwise @c missing
/// @tparam OptionalOrigin the type of the quantity origin for @c Derived if there is one, otherwise @c missing
/// @tparam OptionalUnit the type of the default unit for @c Derived if there is one, otherwise @c missing
template <typename Derived, typename OptionalParentKind, typename OptionalOrigin, typename OptionalUnit>
class quantity_kind_base
    : public quantity_kind_parent_mixin<OptionalParentKind>
    , public quantity_kind_origin_mixin<OptionalOrigin>
    , public quantity_kind_unit_mixin<OptionalUnit> {
 public:
  /// @brief Type alias to facilitate detecting quantity kinds
  using quantity_kind_type = Derived;

  /// @brief Alias for a quantity kind which is the quantity kind @c Derived with the specified @c Exponent
  /// @tparam Exponent The exponent to use
  template <units_detail::exponent_t Exponent>
  using with_exponent = kind_with_exponent_t<Derived, Exponent>;

  /// @brief Alias for a quantity kind with @c Derived as the parent kind and a specified unit
  /// @tparam OtherUnit selected unit for quantity kind
  template <typename OtherUnit>
  using in = std::enable_if_t<
      std::is_class<OtherUnit>::value,
      decltype(rebind_quantity_kind_unit_type(std::declval<quantity_kind_base>(), std::declval<OtherUnit>()))>;
};

/// @brief Class to hold a bunch of reusable sanity checks for the specializations of @c choose_quantity_kind_base
/// @tparam Derived the user-supplied derived type attempting to inherit from the @c quantity_kind class
/// @tparam OptionalParentKind the parent kind chosen by @c choose_quantity_kind_base or @c missing if none
/// @tparam OptionalOrigin the origin chosen by @c choose_quantity_kind_base or @c missing if none
/// @tparam OptionalUnit the unit chosen by @c choose_quantity_kind_base or @c missing if none
template <typename Derived, typename OptionalParentKind, typename OptionalOrigin, typename OptionalUnit>
class choose_quantity_kind_sanity_check {
  static_assert(
      is_missing_v<OptionalParentKind> || is_quantity_kind_v<OptionalParentKind>,
      "The specified parent kind must be a quantity kind"
  );
  static_assert(
      is_missing_v<OptionalParentKind> || !is_unit_v<OptionalParentKind>,
      "The specified parent kind must not be a unit"
  );

  static_assert(
      is_missing_v<OptionalOrigin> || is_quantity_origin_v<OptionalOrigin>,
      "The specified origin must be a quantity origin"
  );

  static_assert(is_missing_v<OptionalUnit> || is_unit_v<OptionalUnit>, "The specified unit must be a quantity unit");
  static_assert(
      is_missing_v<OptionalUnit> || units_detail::is_declared_possible_unit_for_v<OptionalUnit, Derived> ||
          is_possible_unit_for_v<OptionalUnit, OptionalParentKind>,
      "The specified unit must be a possible unit for the specified kind or for its parent kind"
  );
};

/// @brief Get the origin that should be used for a derived quantity kind with a given parent and requested origin
/// @tparam MaybeParentKind either the parent kind of the derived quantity kind, or @c missing
/// @tparam MaybeDerivedOrigin either the requested origin of the derived quantity kind, or @c missing
template <typename MaybeParentKind, typename MaybeDerivedOrigin, typename = constraints<>>
class derived_origin_impl {
 public:
  /// @brief When the parent kind has no origin and no origin was requested, the derived kind has no origin
  using type = units_detail::missing;
};

/// @brief Get the origin that should be used for a derived quantity kind with a given parent and requested origin
/// @tparam MaybeParentKind either the parent kind of the derived quantity kind, or @c missing
/// @tparam MaybeDerivedOrigin either the requested origin of the derived quantity kind, or @c missing
template <typename MaybeParentKind>
class derived_origin_impl<
    MaybeParentKind,
    units_detail::missing,
    constraints<std::enable_if_t<quantity_has_origin_v<MaybeParentKind>>>> {
 public:
  /// @brief When the parent kind has an origin and no origin was requested, the derived kind has the parent's origin
  using type = typename MaybeParentKind::origin;
};

/// @brief Get the origin that should be used for a derived quantity kind with a given parent and requested origin
/// @tparam MaybeParentKind either the parent kind of the derived quantity kind, or @c missing
/// @tparam MaybeDerivedOrigin either the requested origin of the derived quantity kind, or @c missing
template <typename MaybeParentKind, typename MaybeDerivedOrigin>
class derived_origin_impl<
    MaybeParentKind,
    MaybeDerivedOrigin,
    constraints<std::enable_if_t<!quantity_has_origin_v<MaybeParentKind>>>> {
 public:
  /// @brief When the parent kind has no origin and an origin was requested, the derived kind has the requested origin
  using type = MaybeDerivedOrigin;
};

/// @brief Get the origin that should be used for a derived quantity kind with a given parent and requested origin
/// @tparam MaybeParentKind either the parent kind of the derived quantity kind, or @c missing
/// @tparam MaybeDerivedOrigin either the requested origin of the derived quantity kind, or @c missing
template <typename MaybeParentKind, typename MaybeDerivedOrigin>
class derived_origin_impl<
    MaybeParentKind,
    MaybeDerivedOrigin,
    constraints<std::enable_if_t<quantity_has_origin_v<MaybeParentKind>>>> {
  static_assert(
      quantity_origin_detail::origin_is_relative_to_v<MaybeDerivedOrigin, typename MaybeParentKind::origin>,
      "If the parent of a derived quantity kind has an origin, the derived kind's origin must be relative to it"
  );

 public:
  /// @brief When the parent kind has an origin and an origin was requested, the derived kind has the requested origin
  using type = MaybeDerivedOrigin;
};

/// @brief Get the origin that should be used for a derived quantity kind with a given parent and requested origin
/// @tparam MaybeParentKind either the parent kind of the derived quantity kind, or @c missing
/// @tparam MaybeDerivedOrigin either the requested origin of the derived quantity kind, or @c missing
template <typename MaybeParentKind, typename MaybeDerivedOrigin>
using derived_origin_t = typename derived_origin_impl<MaybeParentKind, MaybeDerivedOrigin>::type;

/// @brief Helper class to choose the base class for defining a quantity_kind based on the provided template parameters
/// @tparam Derived the derived class which represents the quantity kind
/// @tparam ParentKindOrOriginOrUnit the type of the parent quantity kind, origin, or default unit for <c>Derived</c>,
/// if any
/// @tparam OriginOrUnit the type of the origin or default unit for <c>Derived</c>, if any
/// @tparam Unit the type of the default unit for <c>Derived</c>, if any
/// @note This is the base specialization, chosen when all three optional parameters are present (not <c>missing</c>)
template <typename Derived, typename ParentKindOrOriginOrUnit, typename OriginOrUnit, typename Unit>
class choose_quantity_kind_base {
  /// @brief In the base case where all parameters are present, the parent is the first optional parameter
  using actual_parent = ParentKindOrOriginOrUnit;

  /// @brief In the base case where all parameters are present, the origin is the second optional parameter
  using actual_origin = OriginOrUnit;

  /// @brief In the base case where all parameters are present, the unit is the third optional parameter
  using actual_unit = Unit;

  // parasoft-begin-suppress AUTOSAR-M0_1_3-c "This sanity check variable is 'used' simply by instantiating it"
  /// @brief Instantiate the sanity check struct that does some static assertions on the arguments
  static constexpr choose_quantity_kind_sanity_check<Derived, actual_parent, actual_origin, actual_unit> check{};
  // parasoft-end-suppress AUTOSAR-M0_1_3-c

 public:
  /// @brief A type alias for the base class to use
  using type = quantity_kind_base<Derived, actual_parent, derived_origin_t<actual_parent, actual_origin>, actual_unit>;
};

/// @brief Helper class to choose the base class for defining a quantity_kind based on the provided template parameters
/// @tparam Derived the derived class which represents the quantity kind
/// @tparam ParentKindOrOriginOrUnit the type of the parent quantity kind, origin, or default unit for <c>Derived</c>,
/// if any
/// @tparam OriginOrUnit the type of the origin or default unit for <c>Derived</c>, if any
/// @tparam Unit the type of the default unit for <c>Derived</c>, if any
/// @note This is the two-argument specialization, chosen when two optional parameters are present (not <c>missing</c>)
template <typename Derived, typename ParentKindOrOrigin, typename OriginOrUnit>
class choose_quantity_kind_base<Derived, ParentKindOrOrigin, OriginOrUnit, units_detail::missing> {
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'missing' does not hide anything"
  /// @brief Type alias for @c missing because it's used several times
  using missing = units_detail::missing;
  // parasoft-end-suppress AUTOSAR-A2_10_1-

  /// @brief The actual parent kind specified for this quantity kind, or @c missing if there is none
  using actual_parent =
      typename std::conditional_t<!is_quantity_origin_v<ParentKindOrOrigin>, ParentKindOrOrigin, missing>;

  /// @brief The actual origin specified for this quantity kind, or @c missing if there is none
  using actual_origin = typename std::conditional_t<
      !is_quantity_kind_v<ParentKindOrOrigin>,
      ParentKindOrOrigin,
      std::conditional_t<!is_unit_v<OriginOrUnit>, OriginOrUnit, missing>>;

  /// @brief The actual unit type specified for this quantity kind, or @c missing if there is none
  using actual_unit = typename std::conditional_t<!is_quantity_origin_v<OriginOrUnit>, OriginOrUnit, missing>;

  // parasoft-begin-suppress AUTOSAR-M0_1_3-c "This sanity check variable is 'used' simply by instantiating it"
  /// @brief Instantiate the sanity check struct that does some static assertions on the arguments
  static constexpr choose_quantity_kind_sanity_check<Derived, actual_parent, actual_origin, actual_unit> check{};
  // parasoft-end-suppress AUTOSAR-M0_1_3-c

 public:
  /// @brief A type alias for the base class to use
  using type = quantity_kind_base<Derived, actual_parent, derived_origin_t<actual_parent, actual_origin>, actual_unit>;
};

/// @brief Helper class to choose the base class for defining a quantity_kind based on the provided template parameters
/// @tparam Derived the derived class which represents the quantity kind
/// @tparam ParentKindOrOriginOrUnit the type of the parent quantity kind, origin, or default unit for <c>Derived</c>,
/// if any
/// @tparam OriginOrUnit the type of the origin or default unit for <c>Derived</c>, if any
/// @tparam Unit the type of the default unit for <c>Derived</c>, if any
/// @note This is the one-argument specialization, chosen when one optional parameter is present (not <c>missing</c>)
template <typename Derived, typename ParentKindOrOriginOrUnit>
class choose_quantity_kind_base<Derived, ParentKindOrOriginOrUnit, units_detail::missing, units_detail::missing> {
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'missing' does not hide anything"
  /// @brief Type alias for @c missing because it's used several times
  using missing = units_detail::missing;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  /// @brief The actual parent kind specified for this quantity kind, or @c missing if there is none
  using actual_parent = typename std::conditional_t<
      !is_quantity_origin_v<ParentKindOrOriginOrUnit> && !is_unit_v<ParentKindOrOriginOrUnit>,
      ParentKindOrOriginOrUnit,
      missing>;

  /// @brief The actual origin specified for this quantity kind, or @c missing if there is none
  using actual_origin =
      typename std::conditional_t<is_quantity_origin_v<ParentKindOrOriginOrUnit>, ParentKindOrOriginOrUnit, missing>;

  /// @brief The actual unit type specified for this quantity kind, or @c missing if there is none
  using actual_unit =
      typename std::conditional_t<is_unit_v<ParentKindOrOriginOrUnit>, ParentKindOrOriginOrUnit, missing>;

  // parasoft-begin-suppress AUTOSAR-M0_1_3-c "This sanity check variable is 'used' simply by instantiating it"
  /// @brief Instantiate the sanity check struct that does some static assertions on the arguments
  static constexpr choose_quantity_kind_sanity_check<Derived, actual_parent, actual_origin, actual_unit> check{};
  // parasoft-end-suppress AUTOSAR-M0_1_3-c

 public:
  /// @brief A type alias for the base class to use
  using type = quantity_kind_base<Derived, actual_parent, derived_origin_t<actual_parent, actual_origin>, actual_unit>;
};

/// @brief Helper class to choose the base class for defining a quantity_kind based on the provided template parameters
/// @tparam Derived the derived class which represents the quantity kind
/// @tparam ParentKindOrOriginOrUnit the type of the parent quantity kind, origin, or default unit for <c>Derived</c>,
/// if any
/// @tparam OriginOrUnit the type of the origin or default unit for <c>Derived</c>, if any
/// @tparam Unit the type of the default unit for <c>Derived</c>, if any
/// @note This is the zero-argument specialization, chosen when all optional parameters are set to @c missing
template <typename Derived>
class choose_quantity_kind_base<Derived, units_detail::missing, units_detail::missing, units_detail::missing> {
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: 'missing' does not hide anything"
  /// @brief Type alias for @c missing because it's used several times
  using missing = units_detail::missing;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

 public:
  /// @brief A type alias for the base class to use
  using type = quantity_kind_base<Derived, missing, missing, missing>;
};

}  // namespace quantity_kind_detail

/// @brief ParentKind class for declaring a quantity kind with a parent quantity kind
/// @tparam Derived the derived class which represents the quantity kind
/// @tparam ParentKindOrOriginOrUnit optional; the type of the parent quantity kind for @c Derived, or the type of the
/// quantity origin, or the type of the default unit
/// @tparam OriginOrUnit optional; the type of the quantity origin, or the default unit. Must not be specified if
/// @c ParentKindOrOriginOrUnit is a unit, and must not be an origin if @c ParentKindOrOriginOrUnit is an origin
/// @tparam Unit the type of the default unit. Must not be specified if either of @c ParentKindOrOriginOrUnit or
/// @c OriginOrUnit is a unit
///
/// @pre @c Derived must derive from @c quantity_kind<Derived, ParentKindOrUnit, Unit>
template <
    typename Derived,
    typename ParentKindOrOriginOrUnit = units_detail::missing,
    typename OriginOrUnit = units_detail::missing,
    typename Unit = units_detail::missing>
using quantity_kind = typename quantity_kind_detail::choose_quantity_kind_base<
    Derived,
    std::remove_cv_t<ParentKindOrOriginOrUnit>,
    std::remove_cv_t<OriginOrUnit>,
    std::remove_cv_t<Unit>>::type;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_QUANTITY_KIND_HPP_
