// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_VOID_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_VOID_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_same.hpp"
#include "stdlib/include/stdlib_detail/remove_cv.hpp"

namespace std {

/// @brief A type trait to check if a type is @c void. The value is @c true if @c Type is @c void, @c false otherwise.
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_void_v = is_same_v<void, remove_cv_t<Type>>;

/// @brief A type trait to check if a type is @c void. The class is derived from @c true_type if @c Type is @c void, @c
/// false_type otherwise.
/// @tparam Type The type to check
template <typename Type>
class is_void : public bool_constant<is_void_v<Type>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_VOID_HPP_
