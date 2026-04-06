// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_UNITS_CONVERSION_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_UNITS_CONVERSION_TRAITS_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/ratio.hpp"
#include "arene/base/type_list/apply_all.hpp"
#include "arene/base/units/base_kind_set.hpp"
#include "arene/base/units/combine_kinds.hpp"
#include "arene/base/units/explicit_base_set.hpp"
#include "arene/base/units/is_base_quantity_kind.hpp"
#include "arene/base/units/is_explicit.hpp"
#include "arene/base/units/is_implicit.hpp"
#include "arene/base/units/is_quantity_kind.hpp"

namespace arene {
namespace base {
namespace units_conversion_traits_detail {

/// @brief determine if a quantity kind type is explicit and has an implicit parent
/// @tparam Kind quantity kind type
template <typename Kind, typename = constraints<>>
extern constexpr bool is_explicit_with_implicit_parent_v = false;

/// @brief determine if a quantity kind type is explicit and has an implicit parent
/// @tparam Kind quantity kind type
template <typename Kind>
extern constexpr bool is_explicit_with_implicit_parent_v<
    Kind,
    constraints<
        std::enable_if_t<units_detail::is_explicit_v<Kind>>,
        std::enable_if_t<units_detail::is_implicit_v<typename Kind::parent_quantity_kind_type>>>> = true;

/// @brief determine if a type provides a @c scale_factor typedef
/// @tparam Type type
template <typename Type, typename = constraints<>>
extern constexpr bool has_scale_factor_v = false;

/// @brief determine if a type provides a @c scale_factor typedef
/// @tparam Type type
template <typename Type>
extern constexpr bool has_scale_factor_v<Type, constraints<typename Type::scale_factor>> = true;

/// @brief Determine if the quantity type can be implicitly converted
/// @tparam From the source type of the unit being impliclty converted
/// @tparam To the destination type of the unit being implicitly converted
///
/// This primary template defines @c From to *not* be implicitly convertible to @c To
template <typename From, typename To, typename = constraints<>>
class units_conversion_traits_impl {
 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is not 'member data', it is a public property,
  // exposed as part of the API"
  /// @brief Boolean value that is @c true if these types "compatible" such that quantities with type @c From can be
  /// implicitly converted to quantities of type @c To, @c false otherwise.
  static constexpr bool compatible{false};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
};

/// @brief Determine if the quantity type can be implicitly converted
/// @tparam From the source and destination type of the unit being impliclty converted
///
/// This specialization allows a type to always be compatible with the same type.
template <typename From>
class units_conversion_traits_impl<From, From, constraints<std::enable_if_t<!is_quantity_kind_v<From>>>> {
 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is not 'member data', it is a public property,
  // exposed as part of the API"
  /// @brief Boolean value that is @c true to indicate that a type can be "converted" to itself
  static constexpr bool compatible{true};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a

  /// @brief The scale factor to use when converting a type to itself: no scaling
  using scale_factor = std::ratio<1, 1>;
};

/// @brief obtain the destination base set
/// @tparam Kind quantity kind type
template <typename Kind, typename = constraints<>>
struct destination_base_set {
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is not 'member data', it is a public property,
  // exposed as part of the API"
  // parasoft-begin-suppress AUTOSAR-A3_3_2-a "False positive: variable 'value' is initialized"
  // parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variable 'value' is initialized"
  /// @brief destination explicit base set
  static constexpr auto value = units_detail::explicit_base_set<Kind>;
  // parasoft-end-suppress AUTOSAR-A3_3_2-a
  // parasoft-end-suppress CERT_CPP-DCL56-a
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
};

/// @brief obtain the destination base set
/// @tparam Kind quantity kind type
template <typename Kind>
struct destination_base_set<Kind, constraints<std::enable_if_t<is_explicit_with_implicit_parent_v<Kind>>>> {
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is not 'member data', it is a public property,
  // exposed as part of the API"
  // parasoft-begin-suppress AUTOSAR-A3_3_2-a "False positive: variable 'value' is initialized"
  // parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variable 'value' is initialized"
  /// @brief destination explicit base set
  static constexpr auto value = units_detail::explicit_base_set<typename Kind::parent_quantity_kind_type>;
  // parasoft-end-suppress AUTOSAR-A3_3_2-a
  // parasoft-end-suppress CERT_CPP-DCL56-a
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
};

/// @brief Determine if the quantity type can be implicitly converted
/// @tparam From the source type of the unit being impliclty converted
/// @tparam To the destination type of the unit being implicitly converted
///
/// This specialization allows a quantity to be compatible with another quantity
/// if the quantity kind base sets of @c From and @c To are compatible.
///
/// @note This specialization only applies if both quantity kind types do *not*
///   explicitly specify a @c scale_factor typedef.
template <typename From, typename To>
class units_conversion_traits_impl<
    From,
    To,
    constraints<
        std::enable_if_t<is_quantity_kind_v<From>>,
        std::enable_if_t<is_quantity_kind_v<To>>,
        std::enable_if_t<!has_scale_factor_v<From>>,
        std::enable_if_t<!has_scale_factor_v<To>>>> {
  /// @brief defines the source explicit base set
  struct source_set {
    // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is not 'member data', it is a public property,
    // exposed as part of the API"
    // parasoft-begin-suppress AUTOSAR-A3_3_2-a "False positive: variable 'value' is initialized"
    // parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variable 'value' is initialized"
    /// @brief source explicit base set
    static constexpr auto value = units_detail::explicit_base_set<From>;
    // parasoft-end-suppress AUTOSAR-A3_3_2-a
    // parasoft-end-suppress CERT_CPP-DCL56-a
    // parasoft-end-suppress AUTOSAR-M11_0_1-a
  };

  /// @brief source explicit base set converted to an implicit combined quantity kind
  using lifted_source_set =
      type_lists::apply_all_t<units_detail::as_list_of_kinds_with_exponents_t<source_set>, combine_kinds_t>;
  /// @brief dest explicit base set converted to an implicit combined quantity kind
  using lifted_dest_set = type_lists::
      apply_all_t<units_detail::as_list_of_kinds_with_exponents_t<destination_base_set<To>>, combine_kinds_t>;

 public:
  /// @brief indicates if @c From and @c To are compatible quantity kind types
  ///
  /// Boolean value that is @c true to indicate that the explicit bases
  /// of @c From is a relaxation of the @c Ed, where :
  /// * if @c To is an explicit quantity kind and
  ///   @c To::parent_quantity_kind_type is an implicit quantity kind,
  ///   @c Ed is the explicit base set of @c To::parent_quantity_kind_type
  /// * otherwise, @c Ed is the explicit base set of @c To.
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: this is not 'member data', it is a public property,
  // exposed as part of the API"
  // parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variable 'compatible' is initialized"
  static constexpr bool
      compatible{std::is_same<lifted_source_set, lifted_dest_set>::value || is_base_quantity_kind_of_v<lifted_dest_set, lifted_source_set>};
  // parasoft-end-suppress CERT_CPP-DCL56-a
  // parasoft-end-suppress AUTOSAR-M11_0_1-a

  /// @brief The scale factor to use when converting quantity kind types without explicit scaling factors
  using scale_factor = std::ratio<1, 1>;
};

}  // namespace units_conversion_traits_detail

/// @brief Traits class with information about if and how a physical unit of type @c From can be converted to a physical
/// unit of type @c To.
///
/// The physical unit types themselves are just tags used for the @c quantity class template rather than holding any
/// values directly.
///
/// @tparam From the tag type of the unit being converted from
/// @tparam To the tag type of the unit being converted to
template <typename From, typename To>
class units_conversion_traits : public units_conversion_traits_detail::units_conversion_traits_impl<From, To> {};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_UNITS_CONVERSION_TRAITS_HPP_
