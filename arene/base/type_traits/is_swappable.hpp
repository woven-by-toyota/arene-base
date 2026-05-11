// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_SWAPPABLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_SWAPPABLE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/substitution_succeeds.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"

namespace arene {
namespace base {

namespace swappable_detail {

/// @brief Poison pill definition of @c swap
///
/// This is to ensure that @c use_adl_swap only looks for a function declaration and does not pick up some other global
/// object named @c swap.
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
template <class>
auto swap() -> void = delete;
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

/// @brief helper type trait for checking if ADL swap is valid, aliased to the result type of such an invocation
template <typename T, typename U>
using use_adl_swap = decltype(swap(std::declval<T>(), std::declval<U>()));
}  // namespace swappable_detail

///
/// @brief Trait query if a two types have an ADL-discoverable swap function.
///
/// @tparam T The first type to test.
/// @tparam U The second type to test.
/// @return If the expressions @c swap(std::declval<T>(),std::declval<U>()) and @c
/// swap(std::declval<U>(),std::declval<T>()) are both well-formed in unevaluated context after using @c std::swap, the
/// value equals true. Otherwise, the value is false.
///
template <typename T, typename U>
extern constexpr bool is_adl_swappable_with_v = substitution_succeeds<swappable_detail::use_adl_swap, T, U> &&
                                                substitution_succeeds<swappable_detail::use_adl_swap, U, T>;
///
/// @brief Trait query if a type has an ADL-discoverable swap function.
///
/// @tparam T The type to test.
/// @return true if an unqualified call to @c swap(T&,T&) is well-formed.
///
template <typename T>
extern constexpr bool is_adl_swappable_v = substitution_succeeds<swappable_detail::use_adl_swap, T&, T&>;

/// @{
///
/// @brief Trait query if two types have a noexcept ADL-discoverable swap function.
///
/// @tparam T The first type to test.
/// @tparam U The second type to test.
/// @return If the expressions @c swap(std::declval<T>(),std::declval<U>()) and @c
/// swap(std::declval<U>(),std::declval<T>()) are both well-formed in unevaluated context after using @c std::swap and
/// are marked @c noexcept, the value equals true. Otherwise, the value is false.
///
template <typename T, typename U, bool = is_adl_swappable_with_v<T, U>>
extern constexpr bool is_nothrow_adl_swappable_with_v = false;

template <typename T, typename U>
extern constexpr bool is_nothrow_adl_swappable_with_v<T, U, true> =
    noexcept(swap(std::declval<T>(), std::declval<U>()))&& noexcept(swap(std::declval<U>(), std::declval<T>()));
/// @}

/// @brief Trait query if a type has a noexcept ADL-discoverable swap function.
///
/// @tparam T The type to test.
/// @return true if an unqualified call to @c swap(T&,T&) is well-formed and it is marked @c noexcept .
///
template <typename T, bool = is_adl_swappable_v<T>>
extern constexpr bool is_nothrow_adl_swappable_v = is_nothrow_adl_swappable_with_v<T&, T&>;

///
/// @brief Trait query if a type can be swapped via the default implementation.
///
/// @tparam T The type to test.
/// @return true if @c std::is_move_assignable<T> and @c std::is_move_constructible<T> are true.
///
template <typename T>
extern constexpr bool is_default_swappable_v =
    std::is_move_assignable<T>::value && std::is_move_constructible<T>::value;

///
/// @brief Trait query if a type can be noexcept swapped the default implementation.
///
/// @tparam T The type to test.
/// @return true if @c std::is_nothrow_move_assignable<T> and @c std::is_nothrow_move_constructible<T> are true.
///
template <typename T>
extern constexpr bool is_nothrow_default_swappable_v =
    std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value;

///
/// @brief Trait query indicating two types can be swapped
///
/// @tparam T the first type to test
/// @tparam U the second type to test
/// @return If the expressions @c swap(std::declval<T>(),std::declval<U>()) and @c
/// swap(std::declval<U>(),std::declval<T>()) are both well-formed in unevaluated context after using @c std::swap, the
/// value equals true. Otherwise, the value is false.
///
template <typename T, class U>
extern constexpr bool is_swappable_with_v =
    is_adl_swappable_with_v<T, U> || (std::is_same<T, U>::value && is_default_swappable_v<std::remove_reference_t<T>>);

///
/// @brief Trait query indicating if a type can be swapped
///
/// @tparam T the type to test
/// @return @c is_swappable_with_v<T&, T&>
///
template <typename T>
extern constexpr bool is_swappable_v = is_swappable_with_v<T&, T&>;

///
/// @brief Trait query indicating if two types can be noexcept-swapped
///
/// @tparam T the first type to test
/// @tparam U the second type to test
/// @return If the expressions @c swap(std::declval<T>(),std::declval<U>()) and @c
/// swap(std::declval<U>(),std::declval<T>()) are both well-formed in unevaluated context after using @c std::swap and
/// are marked @c noexcept, the value equals true. Otherwise, the value is false.
///
template <typename T, typename U>
extern constexpr bool is_nothrow_swappable_with_v =
    is_nothrow_adl_swappable_with_v<T, U> ||
    (std::is_same<T, U>::value && !is_adl_swappable_v<T> && is_nothrow_default_swappable_v<std::remove_reference_t<T>>);

///
/// @brief Trait query indicating if a type can be noexcept-swapped
///
/// @tparam T the type to test
/// @return @c is_nothrow_swappable_with_v<T&, T&>
///
template <typename T>
extern constexpr bool is_nothrow_swappable_v = is_nothrow_swappable_with_v<T&, T&>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_SWAPPABLE_HPP_
