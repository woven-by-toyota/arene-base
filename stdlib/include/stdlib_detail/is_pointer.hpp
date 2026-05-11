// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_POINTER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_POINTER_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/remove_cv.hpp"

namespace std {

namespace is_pointer_detail {

/// @brief Base case when @c T is not a pointer
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_pointer_impl = false;

/// @brief Case when @c T is a pointer
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_pointer_impl<T*> = true;

}  // namespace is_pointer_detail

/// @brief Check if the supplied template argument is a pointer or not. The
///     value is @c true if so, @c false otherwise
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_pointer_v = is_pointer_detail::is_pointer_impl<remove_cv_t<T>>;

/// @brief Check if the supplied template argument is a pointer or not. The
///     value is @c true if so, @c false otherwise
/// @tparam T The type to check
template <typename T>
class is_pointer : public bool_constant<is_pointer_v<T>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_POINTER_HPP_
