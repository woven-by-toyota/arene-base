// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_DETAIL_OPTIONAL_VALUE_INTERFACE_EXCEPTIONS_ENABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_DETAIL_OPTIONAL_VALUE_INTERFACE_EXCEPTIONS_ENABLED_HPP_

// IWYU pragma: private, include "arene/base/optional/detail/optional_value_interface.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/detail/exceptions.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

namespace arene {
namespace base {

/// forward declaration
template <typename T>
class optional;

/// @brief Except for access to the value in an empty optional
class bad_optional_access : public std::runtime_error {
 public:
  // parasoft-begin-suppress AUTOSAR-A15_4_4-a "False-positive: std::runtime_error construct may throw"
  /// @brief Construct an object with a default error message
  bad_optional_access()
      : std::runtime_error("Bad optional access") {}
  // parasoft-end-suppress AUTOSAR-A15_4_4-a
};

namespace optional_detail {

/// @brief Helper CRTP class to provide the @c optional::value() member function when exceptions are enabled
/// @tparam OptionalT The type of the optional to provide the interface for. Must be an instantiation of @c optional
template <typename OptionalT>
class optional_value_interface;

/// @brief Helper CRTP class to provide the @c optional::value() member function when exceptions are enabled
/// @tparam T The value type of the optional the interface is being provided for.
template <typename T>
class optional_value_interface<optional<T>> {
  static_assert(
      detail::are_exceptions_enabled_v,
      "can only be instantiated if exceptions are enabled, this suggests a build configuration error."
  );

  /// @brief Helper to cast @c *this to the derived type
  /// @return optional<T>& @c *this as the derived type
  constexpr auto as_optional() & noexcept -> optional<T>& { return static_cast<optional<T>&>(*this); }
  /// @brief Helper to cast @c *this to the derived type
  /// @return optional<T> const& @c *this as the derived type
  constexpr auto as_optional() const& noexcept -> optional<T> const& { return static_cast<optional<T> const&>(*this); }

  /// @brief Deducing-this style helper for @c optional::value.
  /// @tparam SelfT The qualified result type to invoke with. Must satisfy @c decays_to<Self,optional>
  /// @param self the instance of the result to use.
  /// @return A @c T which is cref-qualified the same as @c self .
  /// @throws bad_optional_access If the optional is the  optional is not engaged.
  template <typename SelfT>
  static constexpr auto do_value(SelfT&& self) -> decltype(auto) {
    // the conditional has to be written with the throw inside the if-block due to a constexpr bug in GCC8 where if the
    // throw is outside a returning if-block it will see the "unguarded" throw and immediately decide this isn't a
    // constant expression for all paths.
    if (!self.has_value()) {
      throw bad_optional_access();
    }
    return *std::forward<SelfT>(self);
  }

 public:
  /// @brief Accesses the value in the optional or throw if the  optional is not engaged.
  /// @return T& A reference to the held value.
  /// @throws bad_optional_access If @c !has_value().
  constexpr auto value() & -> T& { return optional_value_interface::do_value(as_optional()); }
  /// @brief Accesses the value in the optional or throw if the  optional is not engaged.
  /// @return T const& A reference to the held value.
  /// @throws bad_optional_access If @c !has_value().
  constexpr auto value() const& -> T const& { return optional_value_interface::do_value(as_optional()); }
  /// @brief Accesses the value in the optional or throw if the  optional is not engaged.
  /// @return T&& A reference to the held value.
  /// @throws bad_optional_access If @c !has_value().
  constexpr auto value() && -> T&& { return optional_value_interface::do_value(std::move(as_optional())); }
  /// @brief Accesses the value in the optional or throw if the  optional is not engaged.
  /// @return T const&& A reference to the held value.
  /// @throws bad_optional_access If @c !has_value().
  constexpr auto value() const&& -> T const&& {
    // parasoft-begin-suppress AUTOSAR-A18_9_3-a-2 "We want to ensure the qualifier is preserved"
    return optional_value_interface::do_value(std::move(as_optional()));
    // parasoft-end-suppress AUTOSAR-A18_9_3-a-2
  }
};

}  // namespace optional_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_OPTIONAL_DETAIL_OPTIONAL_VALUE_INTERFACE_EXCEPTIONS_ENABLED_HPP_
