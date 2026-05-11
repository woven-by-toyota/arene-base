// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_EMPTY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_EMPTY_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"

namespace std {
/// @brief Type trait to check if a type is an empty type. @c true if it is, @c false otherwise
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_empty_v = __is_empty(Type);

/// @brief Type trait to check if a type is an empty type. Derives from @c std::true_type if it is, @c std::false_type
/// otherwise
/// @tparam Type The type to check
template <typename Type>
class is_empty : public bool_constant<is_empty_v<Type>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_EMPTY_HPP_
