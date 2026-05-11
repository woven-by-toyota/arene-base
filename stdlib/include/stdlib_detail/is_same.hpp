// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_SAME_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_SAME_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"

namespace std {
/// @brief Trait to detect if the two supplied types are the same or not. The value is @c true if the types are the
/// same, @c false otherwise
/// @tparam T1 The first type
/// @tparam T2 The second type
template <typename T1, typename T2>
extern constexpr bool is_same_v = false;

/// @brief Trait to detect if the two supplied types are the same or not. The value is @c true if the types are the
/// same, @c false otherwise
/// @tparam T The type
template <typename T>
extern constexpr bool is_same_v<T, T> = true;

/// @brief Trait to detect if the two template parameters are the same or not. The @c value member is @c true if the
/// types are the same, @c false otherwise
/// @tparam T1 The first type
/// @tparam T2 The second type
template <typename T1, typename T2>
class is_same : public bool_constant<is_same_v<T1, T2>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_SAME_HPP_
