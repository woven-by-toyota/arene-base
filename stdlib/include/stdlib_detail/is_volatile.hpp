// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_VOLATILE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_VOLATILE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"

namespace std {

/// @brief A type trait to check if a type is @c volatile. The value is @c true if @c Type is @c volatile, @c false
/// otherwise.
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_volatile_v = false;

// parasoft-begin-suppress AUTOSAR-A2_11_1-a-2 "This type trait is required to use volatile"
/// @brief A type trait to check if a type is @c volatile. The value is @c true if @c Type is @c volatile, @c false
/// otherwise.
/// @tparam Type The type to check, without @c volatile
template <typename Type>
extern constexpr bool is_volatile_v<Type volatile> = true;
// parasoft-end-suppress AUTOSAR-A2_11_1-a-2

/// @brief A type trait to check if a type is @c volatile. The class is derived from @c true_type if @c Type is @c
/// volatile,
/// @c false_type otherwise.
/// @tparam Type The type to check
template <typename Type>
class is_volatile : public bool_constant<is_volatile_v<Type>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_VOLATILE_HPP_
