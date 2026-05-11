// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_FUNCTION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_FUNCTION_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_const.hpp"
#include "stdlib/include/stdlib_detail/is_reference.hpp"

namespace std {

/// @brief A type trait to check if a type is a @c function. The value is @c true if @c Type is a @c function, @c false
/// otherwise.
/// Adding a cv-qualifier sequence on top of a function type causes the cv-qualifier to be ignored.
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_function_v = not is_const_v<Type const> and not is_reference_v<Type>;

/// @brief A type trait to check if a type is a @c function. The class is derived from @c true_type if @c Type is a
/// @c function, @c false_type otherwise.
/// @tparam Type The type to check
template <typename Type>
class is_function : public bool_constant<is_function_v<Type>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_FUNCTION_HPP_
