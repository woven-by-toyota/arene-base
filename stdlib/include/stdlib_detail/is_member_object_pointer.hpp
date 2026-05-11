// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_MEMBER_OBJECT_POINTER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_MEMBER_OBJECT_POINTER_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/is_function.hpp"
#include "stdlib/include/stdlib_detail/remove_cv.hpp"

namespace std {

namespace is_member_object_pointer_detail {

/// @brief Base case when @c T is not a member pointer
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_member_object_pointer_impl = false;

/// @brief Case when @c T is a member pointer
/// @tparam T The type to check
template <typename T, typename U>
extern constexpr bool is_member_object_pointer_impl<T U::*> = !is_function_v<T>;

}  // namespace is_member_object_pointer_detail

/// @brief Check if the supplied template argument is a member object pointer or
///     not. The value is @c true if so, @c false otherwise
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_member_object_pointer_v =
    is_member_object_pointer_detail::is_member_object_pointer_impl<remove_cv_t<T>>;

/// @brief Check if the supplied template argument is a member object pointer or
///     not. The value is @c true if so, @c false otherwise
/// @tparam T The type to check
template <typename T>
class is_member_object_pointer : public bool_constant<is_member_object_pointer_v<T>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_MEMBER_OBJECT_POINTER_HPP_
