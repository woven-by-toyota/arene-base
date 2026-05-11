// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_UNSIGNED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_UNSIGNED_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_arithmetic.hpp"

namespace std {

namespace is_unsigned_detail {
/// @brief Implementation constant for @c std::is_unsigned_v The value is @c true if @c Type is an unsigned type, @c
/// false otherwise.
/// @tparam Type The type to check
template <typename Type, bool = is_arithmetic_v<Type>>
extern constexpr bool is_unsigned_v = false;

/// @brief Implementation constant for @c std::is_unsigned_v The value is @c true if @c Type is an unsigned type, @c
/// false otherwise.
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_unsigned_v<Type, true> = static_cast<Type>(0) < static_cast<Type>(-1);

}  // namespace is_unsigned_detail

/// @brief A type trait to check if a type is unsigned. The value is @c true if @c Type is an unsigned type, @c false
/// otherwise.
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_unsigned_v = is_unsigned_detail::is_unsigned_v<Type>;

/// @brief A type trait to check if a type is unsigned. The class is derived from @c true_type if @c Type is an unsigned
/// type, @c false_type otherwise.
/// @tparam Type The type to check
template <typename Type>
class is_unsigned : public bool_constant<is_unsigned_v<Type>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_UNSIGNED_HPP_
