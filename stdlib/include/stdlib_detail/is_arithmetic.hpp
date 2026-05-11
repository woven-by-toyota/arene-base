// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_ARITHMETIC_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_ARITHMETIC_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_floating_point.hpp"
#include "stdlib/include/stdlib_detail/is_integral.hpp"

namespace std {

/// @brief A type trait to check if a type is an arithmetic type. The value is @c true if @c Type is an arithmetic type,
/// @c false otherwise.
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_arithmetic_v = is_integral_v<Type> || is_floating_point_v<Type>;

/// @brief A type trait to check if a type is arithmetic. The class is derived from @c true_type if @c Type is an
/// arithmetic type, @c false_type otherwise.
/// @tparam Type The type to check
template <typename Type>
class is_arithmetic : public bool_constant<is_arithmetic_v<Type>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_ARITHMETIC_HPP_
