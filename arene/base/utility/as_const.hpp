// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_AS_CONST_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_AS_CONST_HPP_

// IWYU pragma: private, include "arene/base/utility.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/stdlib_choice/add_const.hpp"

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A7_5_1-a-2 "Temporary objects are handled by a different overload of this function"
/// @brief Obtain an lvalue-reference to a const object of type T.
///
/// Use this as an easier way to obtain a const-reference, deducing the appropriate type compared to writing explicit
/// const_cast expressions.
///
/// @note This is a back-port of the @c std::as_const() function.
///
/// @tparam T The type of the object to add @c const to
/// @param obj The object to obtain a const-reference to.
/// @return A const-lvalue-reference to the argument.
template <typename T>
constexpr auto as_const(T& obj) noexcept -> std::add_const_t<T>& {
  return obj;
}
// parasoft-end-suppress AUTOSAR-A7_5_1-a-2
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

// parasoft-begin-suppress AUTOSAR-M3_3_2 "False positive: inline function used in multiple translation units"
// parasoft-begin-suppress AUTOSAR-A8_4_12 "False positive: T is not deduced as a reference"
/// @brief Prevent passing rvalue arguments to as_const().
/// @tparam T The type of the object to add @c const to
/// @param rvalue The rvalue object
template <typename T>
constexpr void as_const(T const&& rvalue) = delete;
// parasoft-end-suppress AUTOSAR-A8_4_12
// parasoft-end-suppress AUTOSAR-M3_3_2

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_UTILITY_AS_CONST_HPP_
