// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_ARRAY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_ARRAY_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"

namespace std {

/// @brief A type trait to check if a type is an @c array. The value is @c true if @c Type is an @c array, @c false
/// otherwise.
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_array_v = false;

/// @brief A type trait to check if a type is an @c array. The value is @c true if @c Type is an @c array, @c false
/// otherwise.
/// @tparam Type The type to check
template <typename Type>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
extern constexpr bool is_array_v<Type[]> = true;

/// @brief A type trait to check if a type is an @c array. The value is @c true if @c Type is an @c array, @c false
/// otherwise.
/// @tparam Type The type to check
template <typename Type, size_t N>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
extern constexpr bool is_array_v<Type[N]> = true;

/// @brief A type trait to check if a type is an @c array. The class is derived from @c true_type if @c Type is an
/// @c array, @c false_type otherwise.
/// @tparam Type The type to check
template <typename Type>
class is_array : public bool_constant<is_array_v<Type>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_ARRAY_HPP_
