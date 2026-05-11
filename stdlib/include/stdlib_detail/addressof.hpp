// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADDRESSOF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADDRESSOF_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <memory>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A0_1_4-a-2 "False positive: parameter is used"
// parasoft-begin-suppress AUTOSAR-A5_0_3-a "False positive: there is only 1 level of pointer indirection"
/// @brief Obtains the actual address of the object or function arg, even in presence of overloaded operator&.
/// @tparam T The type of the value
/// @param value The value to take the address of
/// @return The address of the supplied value
template <class T>
constexpr auto addressof(T& value) noexcept -> T* {
  return __builtin_addressof(value);
}
// parasoft-end-suppress AUTOSAR-A5_0_3-a
// parasoft-end-suppress AUTOSAR-A0_1_4-a-2
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: Unused parameter does not need to be named"
// parasoft-begin-suppress AUTOSAR-A5_0_3-a "False positive: there is only 1 level of pointer indirection"
/// @brief Obtains the actual address of the object or function arg, even in presence of overloaded operator&.
/// @tparam T The type of the value
/// @return The address of the supplied value
template <class T>
constexpr auto addressof(T const&&) noexcept -> T const* = delete;
// parasoft-end-suppress CERT_C-EXP37-a-3
// parasoft-end-suppress AUTOSAR-A5_0_3-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADDRESSOF_HPP_
