// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_DEFAULT_CONSTRUCTIBLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_DEFAULT_CONSTRUCTIBLE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_constructible.hpp"

namespace std {
/// @brief Type trait to detect if an instance of @c T is default constructible
/// @tparam T The type of the value to check
template <typename T>
extern constexpr bool is_default_constructible_v = is_constructible_v<T>;

/// @brief Type trait to detect if an instance of @c T is default constructible
/// @tparam T The type of the value to check
template <typename T>
class is_default_constructible : public bool_constant<is_default_constructible_v<T>> {};

/// @brief Type trait to detect if an instance of @c T is default constructible without throwing
/// @tparam T The type of the value to check
template <typename T>
extern constexpr bool is_nothrow_default_constructible_v = is_nothrow_constructible_v<T>;

/// @brief Type trait to detect if an instance of @c T is default constructible without throwing
/// @tparam T The type of the value to check
template <typename T>
class is_nothrow_default_constructible : public bool_constant<is_nothrow_default_constructible_v<T>> {};

/// @brief Type trait to detect if an instance of @c T is trivially default constructible
/// @tparam T The type of the value to check
template <typename T>
extern constexpr bool is_trivially_default_constructible_v = __is_trivially_constructible(T);

/// @brief Type trait to detect if an instance of @c T is trivially default constructible
/// @tparam T The type of the value to check
template <typename T>
class is_trivially_default_constructible : public bool_constant<is_trivially_default_constructible_v<T>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_DEFAULT_CONSTRUCTIBLE_HPP_
