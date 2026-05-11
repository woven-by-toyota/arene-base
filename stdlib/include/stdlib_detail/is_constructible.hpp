// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_CONSTRUCTIBLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_CONSTRUCTIBLE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"

namespace std {
/// @brief Type trait to detect if a type is constructible from arguments of the specified types
/// @tparam Type The type to (try to) construct
/// @tparam Args The types of the arguments to (try to) construct from
template <typename Type, typename... Args>
extern constexpr bool is_constructible_v = __is_constructible(Type, Args...);

/// @brief Type trait to detect if a type is constructible from arguments of the specified types
/// @tparam Type The type to (try to) construct
/// @tparam Args The types of the arguments to (try to) construct from
template <typename Type, typename... Args>
class is_constructible : public bool_constant<is_constructible_v<Type, Args...>> {};

namespace is_nothrow_constructible_detail {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: all declarations
// and typedefs *are* preceded by a comment with @brief"

/// @brief Internal implementation of @c is_nothrow_constructible
/// @tparam IsConstructible The result of @c is_constructible_v<Type,Args...>
/// @tparam Type The type to (try to) construct
/// @tparam Args The types of the arguments to (try to) construct from
///
/// This handles the case where @c IsConstructible is @c false
template <bool IsConstructible, typename Type, typename... Args>
class is_nothrow_constructible_impl : public false_type {};

/// @brief Internal implementation of @c is_nothrow_constructible
/// @tparam Type The type to (try to) construct
/// @tparam Args The types of the arguments to (try to) construct from
///
/// This handles the case where @c IsConstructible is @c true
template <typename Type, typename... Args>
class is_nothrow_constructible_impl<true, Type, Args...> : public bool_constant<noexcept(Type(declval<Args>()...))> {};

/// @brief Internal implementation of @c is_nothrow_constructible
/// @tparam Type The type of array to (try to) construct
/// @tparam N The size of array to (try to) construct
/// @tparam Args The types of the arguments to (try to) construct from
///
/// This handles the case where we're checking a C array. A C array is nothrow constructible if the underlying type has
/// a noexcept default constructor.
template <typename Type, size_t N, typename... Args>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
class is_nothrow_constructible_impl<true, Type[N], Args...> : public is_nothrow_constructible_impl<true, Type> {};

/// @brief Internal implementation of @c is_nothrow_constructible
/// @tparam Type The type to (try to) construct
/// @tparam Args The types of the arguments to (try to) construct from
///
/// Delegate to the appropriate @c is_nothrow_constructible_impl specialization depending on the result of @c
/// is_constructible_v
template <typename Type, typename... Args>
class is_nothrow_constructible
    : public is_nothrow_constructible_impl<is_constructible_v<Type, Args...>, Type, Args...> {};

// parasoft-end-suppress AUTOSAR-A2_7_3-a

}  // namespace is_nothrow_constructible_detail

/// @brief Type trait to detect if a type is constructible from arguments of the specified types and is noexcept.
/// @tparam Type The type to (try to) construct
/// @tparam Args The types of the arguments to (try to) construct from
///
/// @note This type trait also implicitly checks if the *destructor* is noexcept.
template <typename Type, typename... Args>
class is_nothrow_constructible : public is_nothrow_constructible_detail::is_nothrow_constructible<Type, Args...> {};

/// @brief Type trait to detect if a type is constructible from arguments of the specified types and is noexcept.
/// @tparam Type The type to (try to) construct
/// @tparam Args The types of the arguments to (try to) construct from
///
/// @note This type trait also implicitly checks if the *destructor* is noexcept.
template <typename Type, typename... Args>
extern constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<Type, Args...>::value;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_CONSTRUCTIBLE_HPP_
