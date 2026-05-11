// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_CLASS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_CLASS_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"

namespace std {

/// @brief A type trait to check if a type is a @c class. The value is @c true if @c Type is a @c class, @c false
/// otherwise.
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_class_v = __is_class(Type);

/// @brief A type trait to check if a type is a @c class. The class is derived from @c true_type if @c Type is a
/// @c class, @c false_type otherwise.
/// @tparam Type The type to check
template <typename Type>
class is_class : public bool_constant<is_class_v<Type>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_CLASS_HPP_
