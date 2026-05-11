// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_EXCHANGE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_EXCHANGE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <utility>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

#include "stdlib/include/stdlib_detail/forward.hpp"
#include "stdlib/include/stdlib_detail/is_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_move_constructible.hpp"
#include "stdlib/include/stdlib_detail/move.hpp"

namespace std {
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Assign a new value to an object and return the old value. Implements @c std::exchange
/// @tparam T The type of the object to update
/// @tparam U The type of the source
/// @param target The object to update
/// @param source The source
/// @return The old value
/// @pre @c T must be move-constructible
/// @pre @c is_assignable_v<T&,U&&> must be true
template <typename T, typename U = T>
constexpr auto
exchange(T& target, U&& source) noexcept(is_nothrow_move_constructible_v<T> && is_nothrow_assignable_v<T&, U&&>) -> T {
  T old{std::move(target)};
  target = std::forward<U>(source);
  return old;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_EXCHANGE_HPP_
