// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTEGRAL_CONSTANT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTEGRAL_CONSTANT_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {
// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: there is a comment with @brief"
/// @brief A helper class to represent a compile-time constant with the specified type and value. Instances of the type
/// are implicitly convertible to the specified type and are nullary-invocable, yielding the specified type. The result
/// of such a conversion or invocation is the specified value. The @c value member is a constant of the specified type,
/// holding the specified value.
/// @tparam Type The type of the value
/// @tparam Value The value of the constant
template <typename Type, Type Value>
class integral_constant {
 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: identifiers can not hide themselves"
  /// @brief The type of this class
  using type = integral_constant;
  /// @brief The type of the value
  using value_type = Type;

  // parasoft-begin-suppress CERT_C-STR34-b-2 "False positive: this is generic code and uses exactly the specified type"
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property"
  /// @brief The constant
  static constexpr value_type value{Value};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a-2
  // parasoft-end-suppress CERT_C-STR34-b-2
  // parasoft-end-suppress AUTOSAR-A2_10_1-e

  // parasoft-begin-suppress AUTOSAR-A8_4_2-a CERT_CPP-MSC52-a-2 CERT_C-MSC37-a "False positive: this function returns a
  // value"
  // parasoft-begin-suppress AUTOSAR-A13_2_3-a-2 "False positive: Suggested return type is incorrect"
  // parasoft-begin-suppress AUTOSAR-A13_5_2-a-2 "Implicit conversion to value_type is part of the API"
  /// @brief Implicit conversion to @c Type
  /// @return The specified @c Value
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  constexpr operator value_type() const noexcept { return value; }
  // parasoft-end-suppress AUTOSAR-A13_5_2-a-2
  // parasoft-end-suppress AUTOSAR-A13_2_3-a-2
  // parasoft-end-suppress AUTOSAR-A8_4_2-a CERT_CPP-MSC52-a-2 CERT_C-MSC37-a

  // parasoft-begin-suppress AUTOSAR-A8_4_2-a CERT_CPP-MSC52-a-2 CERT_C-MSC37-a "False positive: this function returns a
  // value"
  /// @brief Nullary function call operator
  /// @return The specified @c Value
  constexpr auto operator()() const noexcept -> value_type { return value; }
  // parasoft-end-suppress AUTOSAR-A8_4_2-a CERT_CPP-MSC52-a-2 CERT_C-MSC37-a
};
// parasoft-end-suppress AUTOSAR-A2_7_3-a

/// @brief Out-of-line definition for the @c value constant
/// @tparam Type The type of the value
/// @tparam Value The value of the constant
template <typename Type, Type Value>
constexpr Type integral_constant<Type, Value>::value;

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: there is a comment with @brief"

/// @brief An alias for @c integral_constant<bool,Value>
/// @tparam Value the value of the constant
template <bool Value>
using bool_constant = integral_constant<bool, Value>;

// parasoft-end-suppress AUTOSAR-A2_7_3-a

/// @brief An alias for @c bool_constant<true>
using true_type = bool_constant<true>;

/// @brief An alias for @c bool_constant<false>
using false_type = bool_constant<false>;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTEGRAL_CONSTANT_HPP_
