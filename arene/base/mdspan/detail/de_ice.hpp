// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::mdspan_detail::de_ice"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_DE_ICE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_DE_ICE_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/type_traits/is_integral_constant_like.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief function object implementing de_ice
class de_ice_fn  //
{
  /// @brief returns a value from an integral-constant-like
  /// @tparam IntegralConstant integral-constant-like
  /// @return @c decltype((IntegralConstant::value))
  template <class IntegralConstant>
  static constexpr auto impl(std::true_type, IntegralConstant const&) noexcept -> decltype((IntegralConstant::value)) {
    return IntegralConstant::value;
  }

  /// @brief returns a value from a non integral-constant-like
  /// @tparam Type type
  /// @param value function argument
  /// @return reference to @c value
  template <class Type>
  static constexpr auto impl(std::false_type, Type const& value) noexcept -> Type const& {
    // parasoft-begin-suppress AUTOSAR-A7_5_1-a "This function simply forwards the reference"
    return value;
    // parasoft-end-suppress AUTOSAR-A7_5_1-a
  }

 public:
  /// @brief returns a value, possibly from a compile-time constant
  /// @tparam Type type
  /// @param value value
  ///
  /// @return @c Type::value if it is an integral-constant-like, otherwise reference to @c value
  /// @note unlike [mdspan.sub.helpers], returns a reference instead of an object
  template <class Type>
  constexpr auto operator()(Type const& value) const noexcept
      -> decltype(impl(std::integral_constant<bool, is_integral_constant_like_v<Type>>{}, value)) {
    return impl(std::integral_constant<bool, is_integral_constant_like_v<Type>>{}, value);
  }
};

/// @def arene::base::mdspan_detail::de_ice
/// @copydoc arene::base::mdspan_detail::de_ice_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(mdspan_detail::de_ice_fn, de_ice);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_DE_ICE_HPP_
