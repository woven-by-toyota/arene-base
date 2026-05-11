// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_QUANTITY_ORIGIN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_QUANTITY_ORIGIN_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"

namespace arene {
namespace base {

/// @brief CRTP base class used when defining a new quantity origin, e.g. absolute zero for temperature
/// @tparam Derived user-defined class publicly inheriting from this one; @c Derived becomes a quantity origin
template <typename Derived>
class quantity_origin;

/// @brief CRTP base class used when defining a new relative origin, e.g. zero celsius which is relative to zero Kelvin
/// @tparam Derived user-defined class publicly inheriting from this one; @c Derived becomes a quantity origin
/// @tparam BaseOrigin a @c quantity_origin to which this will be defined as being relative
template <typename Derived, typename BaseOrigin>
class relative_origin;

namespace quantity_origin_detail {

// parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

/// @brief Use overload resolution to check if a type is directly derived from @c quantity_origin or @c relative_origin
/// @tparam Origin the type being checked; should be explicitly specified to assert that it's the CRTP parameter
/// @return @c std::true_type so that the selected overload can be examined using @c decltype
template <typename Origin>
constexpr auto is_origin_impl(quantity_origin<Origin>*) noexcept -> std::true_type {
  return {};
}

/// @brief Use overload resolution to check if a type is directly derived from @c quantity_origin or @c relative_origin
/// @tparam Origin the type being checked; should be explicitly specified to assert that it's the CRTP parameter
/// @tparam OtherOrigin any other origin; should be left unspecified and allowed to be deduced
/// @return @c std::true_type so that the selected overload can be examined using @c decltype
template <typename Origin, typename OtherOrigin>
constexpr auto is_origin_impl(relative_origin<Origin, OtherOrigin>*) noexcept -> std::true_type {
  return {};
}

/// @brief Use overload resolution to check if a type is directly derived from @c quantity_origin or @c relative_origin
/// @return @c std::false_type so that the selected overload can be examined using @c decltype
template <typename>
constexpr auto is_origin_impl(void*) noexcept -> std::false_type {
  return {};
}

// parasoft-end-suppress CERT_C-EXP37-a
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Type trait to check if a quantity kind is itself an origin. The value is @c true if it is, @c false otherwise
/// @tparam Kind quantity kind to check
template <typename Origin>
extern constexpr bool is_origin_v = decltype(is_origin_impl<Origin>(std::declval<Origin*>())){};

/// @brief Type trait to check if a quantity kind has an origin. The value is @c true if it does, @c false otherwise
/// @tparam Kind quantity kind to check
template <typename Kind, typename = constraints<>>
extern constexpr bool has_origin_v = false;

/// @brief Type trait to check if a quantity kind has an origin. The value is @c true if it does, @c false otherwise
/// @tparam Kind quantity kind to check
template <typename Kind>
extern constexpr bool has_origin_v<Kind, constraints<typename Kind::origin>> = is_origin_v<typename Kind::origin>;

/// @brief Type trait to check if the first origin is relative to the second (or they're the same)
/// @tparam DerivedOrigin the origin of the derived quantity kind
/// @tparam ParentOrigin the origin of the parent quantity kind
template <typename DerivedOrigin, typename ParentOrigin, typename = constraints<>>
extern constexpr bool origin_is_relative_to_v = std::is_same<ParentOrigin, DerivedOrigin>::value;

/// @brief Type trait to check if the first origin is relative to the second (or they're the same)
/// @tparam DerivedOrigin the origin of the derived quantity kind
/// @tparam ParentOrigin the origin of the parent quantity kind
template <typename DerivedOrigin, typename ParentOrigin>
extern constexpr bool
    origin_is_relative_to_v<DerivedOrigin, ParentOrigin, constraints<typename DerivedOrigin::base_origin>> =
        std::is_same<typename DerivedOrigin::base_origin, typename ParentOrigin::origin>::value;

}  // namespace quantity_origin_detail

/// @brief CRTP base class used when defining a new quantity origin, e.g. absolute zero for temperature
/// @tparam Derived user-defined class publicly inheriting from this one; @c Derived becomes a quantity origin
template <typename Derived>
class quantity_origin {
 public:
  /// @brief Type alias used to identify downstream classes as being anchored to this origin
  using origin = Derived;
};

/// @brief CRTP base class used when defining a new relative origin, e.g. zero celsius which is relative to zero Kelvin
/// @tparam Derived user-defined class publicly inheriting from this one; @c Derived becomes a quantity origin
/// @tparam BaseOrigin a @c quantity_origin to which this will be defined as being relative
template <typename Derived, typename BaseOrigin>
class relative_origin {
  static_assert(
      quantity_origin_detail::is_origin_v<BaseOrigin>,
      "The base origin supplied to relative_origin must actually be a quantity origin"
  );

 public:
  /// @brief Type alias for the base origin to allow checking that the two origins are related
  using base_origin = BaseOrigin;
  /// @brief Type alias used to identify downstream classes as being anchored to this origin
  using origin = Derived;
};

/// @brief Type trait to check if a quantity kind has an origin. The value is @c true if it does, @c false otherwise
/// @tparam Kind Quantity kind to check
template <typename Kind>
extern constexpr bool quantity_has_origin_v = quantity_origin_detail::has_origin_v<Kind>;

/// @brief Type trait to check if a quantity kind is itself an origin. The value is @c true if it is, @c false otherwise
/// @tparam Kind Quantity kind to check
template <typename Kind>
extern constexpr bool is_quantity_origin_v = quantity_origin_detail::is_origin_v<Kind>;

/// @brief Type trait to get the origin of a quantity; if none exists, then this will cause a compilation error
/// @tparam Kind Quantity kind to check
template <typename Kind>
using quantity_origin_t = typename Kind::origin;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UNITS_QUANTITY_ORIGIN_HPP_
