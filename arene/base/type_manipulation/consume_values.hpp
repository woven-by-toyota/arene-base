// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::consume_values"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_CONSUME_VALUES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_CONSUME_VALUES_HPP_

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace consume_values_detail {

/// @brief Type implicit conversions from anything as a nop. Used as a catch-all consumer for @c consume_values
class anything {
 public:
  /// @brief Can default construct
  constexpr anything() noexcept = default;

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a-2 "False positive: Constrained via SFINAE"
  /// @brief Default copy constructor
  /// @param other The source
  constexpr anything(anything const& other) noexcept = default;
  // parasoft-end-suppress AUTOSAR-A13_3_1-a-2
  /// @brief Default move constructor
  /// @param other The source
  constexpr anything(anything&& other) noexcept = default;

  /// @brief Default copy assignment
  /// @param other The source
  /// @return @c *this
  constexpr auto operator=(anything const& other) noexcept -> anything& = default;
  /// @brief Default move assignment
  /// @param other The source
  /// @return @c *this
  constexpr auto operator=(anything&& other) noexcept -> anything& = default;

  /// @brief default destructor
  ~anything() = default;

  /// @brief Can construct from anything.
  /// @tparam Arg the type of the source
  template <typename Arg, constraints<std::enable_if_t<!std::is_same<anything, remove_cvref_t<Arg>>::value>> = nullptr>
  // This constructor works as a copy and move constructor too, but that's OK
  // because the class has no content or behaviour
  // AUTOSAR A12-1-4 exception: The intent of this object is to allow implicit
  // conversions from all values, and discard the values
  // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions,bugprone-forwarding-reference-overload)
  constexpr anything(Arg&&) noexcept {}
};

}  // namespace consume_values_detail

// parasoft-begin-suppress AUTOSAR-M0_1_8-b-2 "The intent of this function is that it does nothing"
/// @brief Consume a list of values and discard them.
/// Must be called with an extra pair of braces around the expressions to be consumed:
/// <c>consume_values({e1,e2,...})</c>. The use of @c std::initializer_list ensures the expressions to be consumed are
/// evaluated in left-to-right sequential order.
/// @pre The supplied expressions must have a non-<c>void</c> type. @c void_ can be used as the return type if it would
/// otherwise be @c void
inline constexpr void consume_values(std::initializer_list<consume_values_detail::anything> const&) noexcept {}
// parasoft-end-suppress AUTOSAR-M0_1_8-b-2

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_MANIPULATION_CONSUME_VALUES_HPP_
