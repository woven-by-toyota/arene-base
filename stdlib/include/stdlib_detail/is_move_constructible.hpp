// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_MOVE_CONSTRUCTIBLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_MOVE_CONSTRUCTIBLE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_constructible.hpp"
#include "stdlib/include/stdlib_detail/is_referenceable.hpp"

namespace std {
namespace is_move_constructible_detail {
/// @brief Type trait to detect if an instance of @c T is move constructible
/// @tparam T The type of the value to check
template <typename T, bool = internal::is_referenceable_v<T>>
extern constexpr bool is_move_constructible_v = false;

/// @brief Type trait to detect if an instance of @c T is move constructible
/// @tparam T The type of the value to check
template <typename T>
extern constexpr bool is_move_constructible_v<T, true> = is_constructible_v<T, T&&>;

/// @brief Type trait to detect if an instance of @c T is move constructible without throwing
/// @tparam T The type of the value to check
template <typename T, bool = internal::is_referenceable_v<T>>
extern constexpr bool is_nothrow_move_constructible_v = false;

/// @brief Type trait to detect if an instance of @c T is move constructible without throwing
/// @tparam T The type of the value to check
template <typename T>
extern constexpr bool is_nothrow_move_constructible_v<T, true> = is_nothrow_constructible_v<T, T&&>;

/// @brief Type trait to detect if an instance of @c T is trivially move constructible
/// @tparam T The type of the value to check
template <typename T, bool = internal::is_referenceable_v<T>>
extern constexpr bool is_trivially_move_constructible_v = false;

/// @brief Type trait to detect if an instance of @c T is trivially move constructible
/// @tparam T The type of the value to check
template <typename T>
extern constexpr bool is_trivially_move_constructible_v<T, true> = __is_trivially_constructible(T, T&&);
}  // namespace is_move_constructible_detail

/// @brief Type trait to detect if an instance of @c T is move constructible
/// @tparam T The type of the value to check
template <typename T>
extern constexpr bool is_move_constructible_v = is_move_constructible_detail::is_move_constructible_v<T>;

/// @brief Type trait to detect if an instance of @c T is move constructible
/// @tparam T The type of the value to check
template <typename T>
class is_move_constructible : public bool_constant<is_move_constructible_v<T>> {};

/// @brief Type trait to detect if an instance of @c T is move constructible without throwing
/// @tparam T The type of the value to check
template <typename T>
extern constexpr bool is_nothrow_move_constructible_v =
    is_move_constructible_detail::is_nothrow_move_constructible_v<T>;

/// @brief Type trait to detect if an instance of @c T is move constructible without throwing
/// @tparam T The type of the value to check
template <typename T>
class is_nothrow_move_constructible : public bool_constant<is_nothrow_move_constructible_v<T>> {};

/// @brief Type trait to detect if an instance of @c T is trivially move constructible
/// @tparam T The type of the value to check
template <typename T>
extern constexpr bool is_trivially_move_constructible_v =
    is_move_constructible_detail::is_trivially_move_constructible_v<T>;

/// @brief Type trait to detect if an instance of @c T is trivially move constructible
/// @tparam T The type of the value to check
template <typename T>
class is_trivially_move_constructible : public bool_constant<is_trivially_move_constructible_v<T>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_MOVE_CONSTRUCTIBLE_HPP_
