// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_RVALUE_REFERENCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_RVALUE_REFERENCE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"

namespace std {
/// @brief Check if the supplied template argument is an rvalue reference or
///     not. The value is @c true if so, @c false otherwise
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_rvalue_reference_v = false;

/// @brief Check if the supplied template argument is an rvalue reference or
///     not. The value is @c true if so, @c false otherwise
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_rvalue_reference_v<T&&> = true;

/// @brief Check if the supplied template argument is an rvalue reference or
///     not. The @c value member is @c true if so, @c false otherwise
/// @tparam T The type to check
template <typename T>
class is_rvalue_reference : public bool_constant<is_rvalue_reference_v<T>> {};
}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_RVALUE_REFERENCE_HPP_
