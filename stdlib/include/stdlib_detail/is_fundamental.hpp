// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_FUNDAMENTAL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_FUNDAMENTAL_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_arithmetic.hpp"
#include "stdlib/include/stdlib_detail/is_same.hpp"
#include "stdlib/include/stdlib_detail/is_void.hpp"
#include "stdlib/include/stdlib_detail/remove_cv.hpp"

namespace std {
/// @brief Check if the supplied template argument is a fundamental type or not.
///     The value is @c true if so, @c false otherwise
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_fundamental_v = is_arithmetic_v<T> || is_void_v<T> || is_same_v<remove_cv_t<T>, nullptr_t>;

/// @brief Check if the supplied template argument is a fundamental type or not.
///     The @c value member is @c true if so, @c false otherwise
/// @tparam T The type to check
template <typename T>
class is_fundamental : public bool_constant<is_fundamental_v<T>> {};
}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_FUNDAMENTAL_HPP_
