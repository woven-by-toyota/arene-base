// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_FORWARD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_FORWARD_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <utility>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/is_lvalue_reference.hpp"
#include "stdlib/include/stdlib_detail/remove_reference.hpp"

namespace std {
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Cast a value to an rvalue if @c T is an rvalue or rvalue reference, otherwise forward an lvalue reference
/// @tparam T The type of the value
/// @param value The value to cast
/// @return The input reference cast to an rvalue
template <typename T>
constexpr auto forward(remove_reference_t<T>& value) noexcept -> T&& {
  return static_cast<T&&>(value);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: Value is forwarded as appropriate"
// parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Value is forwarded as appropriate"
// parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: Value is forwarded as appropriate"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Cast a value to an rvalue if @c T is an rvalue or rvalue reference, otherwise forward an lvalue reference
/// @tparam T The type of the value
/// @param value The value to cast
/// @return The input reference cast to an rvalue
template <typename T>
constexpr auto forward(remove_reference_t<T>&& value) noexcept -> T&& {
  static_assert(!is_lvalue_reference_v<T>, "Cannot forward an rvalue as an lvalue");
  return static_cast<T&&>(value);
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2
// parasoft-end-suppress AUTOSAR-A12_8_4-a-2
// parasoft-end-suppress AUTOSAR-A8_4_5-a-2
// parasoft-end-suppress AUTOSAR-A8_4_6-a-2

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_FORWARD_HPP_
