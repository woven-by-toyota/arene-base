// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MOVE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MOVE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <utility>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/remove_reference.hpp"

namespace std {
// parasoft-begin-suppress AUTOSAR-A8_4_6-a-2 "False positive: Value is moved"
// parasoft-begin-suppress AUTOSAR-A8_4_5-a-2 "False positive: Value is moved"
// parasoft-begin-suppress AUTOSAR-A12_8_4-a-2 "False positive: Value is moved"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A0_1_4-a-2 "False positive: parameter is used"
// parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "False positive: function returns a value"
// parasoft-begin-suppress CERT_C-MSC37-a-2 "False positive: function returns a value"
// parasoft-begin-suppress CERT_CPP-MSC52-a-2 "False positive: function returns a value"
/// @brief Cast a value to an rvalue
/// @tparam T The type of the value
/// @param value The value to cast
/// @return The supplied argument cast to an rvalue reference
template <typename T>
constexpr auto move(T&& value) noexcept -> remove_reference_t<T>&& {
  return static_cast<remove_reference_t<T>&&>(value);
}
// parasoft-end-suppress CERT_CPP-MSC52-a-2
// parasoft-end-suppress CERT_C-MSC37-a-2
// parasoft-end-suppress AUTOSAR-A8_4_2-a-2
// parasoft-end-suppress AUTOSAR-A0_1_4-a-2
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2
// parasoft-end-suppress AUTOSAR-A12_8_4-a-2
// parasoft-end-suppress AUTOSAR-A8_4_5-a-2
// parasoft-end-suppress AUTOSAR-A8_4_6-a-2
}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MOVE_HPP_
