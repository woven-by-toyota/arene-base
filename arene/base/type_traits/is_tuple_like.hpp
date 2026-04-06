// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file is_tuple_like.hpp
/// @brief Provides the @c is_tuple_like_v<T> and @c is_pair_like_v<T> traits
///

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_TUPLE_LIKE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_TUPLE_LIKE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/remove_cv.hpp"
#include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/stdlib_choice/tuple_element.hpp"
#include "arene/base/stdlib_choice/tuple_size.hpp"
#include "arene/base/tuple/detail/get.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace arene {
namespace base {

namespace tuple_like_detail {

/// @brief Readability alias for the type yielded by calling @c get on an instance of @c T
/// @tparam T The CV-qualified type to try getting an element from
/// @tparam Idx The index to check @c get<Idx> for
template <typename T, std::size_t Idx>
using get_result = decltype(tuple_detail::get<Idx>(std::declval<T>()));

/// @brief Type trait to determine if @c tuple_element_t<T> can be constructed from the result of calling @c get on an
/// instance of @c T
/// @tparam T The type to check; types with a member function @c get<Idx>() use that, other types use ADL @c get<Idx>()
/// @tparam Idx The index to check @c get<Idx> for
template <typename T, std::size_t Idx, typename = constraints<>>
extern constexpr bool tuple_element_constructible_from_get = false;

/// @brief Type trait to determine if @c tuple_element_t<T> can be constructed from the result of calling @c get on an
/// instance of @c T
/// @tparam T The type to check; types with a member function @c get<Idx>() use that, other types use ADL @c get<Idx>()
/// @tparam Idx The index to check @c get<Idx> for
template <typename T, std::size_t Idx>
extern constexpr bool tuple_element_constructible_from_get<
    T,
    Idx,
    constraints<std::enable_if_t<
        std::is_constructible<std::tuple_element_t<Idx, T>, get_result<T&, Idx>>::value &&
        std::is_constructible<std::tuple_element_t<Idx, T const>, get_result<T const&, Idx>>::value &&
        std::is_constructible<std::tuple_element_t<Idx, T>, get_result<T&&, Idx>>::value &&
        std::is_constructible<std::tuple_element_t<Idx, T const>, get_result<T const&&, Idx>>::value  //
        >>> = true;

}  // namespace tuple_like_detail

/// @brief Type trait to check if a type implements the tuple protocol
/// @note A type implements the tuple protocol if it has @c get<Idx>() functions and specializations of
/// @c std::tuple_size and <c>std::tuple_element</c>. The @c get<Idx>() implementations can either be member functions
/// or free functions found by ADL; if both are present, the member functions are preferred.
///
/// Types implementing the tuple protocol can be used with structured bindings. Standard classes like <c>std::tuple</c>,
/// <c>std::pair</c>, and <c>std::array</c> implement the tuple protocol, as does <c>arene::base::array</c>.
///
/// This trait checks that @c get<0>() and @c get<size-1>() both return the types specified by <c>tuple_element</c>
/// with the same index. Intermediate indices are not checked for performance reasons. If <c>tuple_size<T> == 0</c>, the
/// type is considered to be tuple-like regardless of the presence or behaviour of @c get and <c>tuple_element</c>.
/// @tparam T The type to check
template <typename T, typename = constraints<>>
extern constexpr bool is_tuple_like_v = false;

/// @brief Type trait to check if a type implements the tuple protocol
/// @note A type implements the tuple protocol if it has @c get<Idx>() functions and specializations of
/// @c std::tuple_size and <c>std::tuple_element</c>. The @c get<Idx>() implementations can either be member functions
/// or free functions found by ADL; if both are present, the member functions are preferred.
///
/// Types implementing the tuple protocol can be used with structured bindings. Standard classes like <c>std::tuple</c>,
/// <c>std::pair</c>, and <c>std::array</c> implement the tuple protocol, as does <c>arene::base::array</c>.
///
/// This trait checks that @c get<0>() and @c get<size-1>() both return the types specified by <c>tuple_element</c>
/// with the same index. Intermediate indices are not checked for performance reasons. If <c>tuple_size<T> == 0</c>, the
/// type is considered to be tuple-like regardless of the presence or behaviour of @c get and <c>tuple_element</c>.
/// @tparam T The type to check
template <typename T>
extern constexpr bool
    is_tuple_like_v<T, constraints<std::enable_if_t<std::tuple_size<remove_cvref_t<T>>::value == 0U>>> = true;

/// @brief Type trait to check if a type implements the tuple protocol
/// @note A type implements the tuple protocol if it has @c get<Idx>() functions and specializations of
/// @c std::tuple_size and <c>std::tuple_element</c>. The @c get<Idx>() implementations can either be member functions
/// or free functions found by ADL; if both are present, the member functions are preferred.
///
/// Types implementing the tuple protocol can be used with structured bindings. Standard classes like <c>std::tuple</c>,
/// <c>std::pair</c>, and <c>std::array</c> implement the tuple protocol, as does <c>arene::base::array</c>.
///
/// This trait checks that @c get<0>() and @c get<size-1>() both return the types specified by <c>tuple_element</c>
/// with the same index. Intermediate indices are not checked for performance reasons. If <c>tuple_size<T> == 0</c>, the
/// type is considered to be tuple-like regardless of the presence or behaviour of @c get and <c>tuple_element</c>.
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_tuple_like_v<
    T,
    constraints<
        std::enable_if_t<std::tuple_size<remove_cvref_t<T>>::value != 0U>,
        std::enable_if_t<tuple_like_detail::tuple_element_constructible_from_get<remove_cvref_t<T>, 0U>>,
        std::enable_if_t<tuple_like_detail::tuple_element_constructible_from_get<
            remove_cvref_t<T>,
            std::tuple_size<remove_cvref_t<T>>::value - 1U>>>> = true;

/// @brief Type trait to check if a type implements the tuple protocol and has a @c std::tuple_size of 2
/// @tparam T The type to check
template <typename T, typename = constraints<>>
extern constexpr bool is_pair_like_v = false;

/// @brief Type trait to check if a type implements the tuple protocol and has a @c std::tuple_size of 2
/// @tparam T The type to check
template <typename T>
extern constexpr bool
    is_pair_like_v<T, constraints<std::enable_if_t<std::tuple_size<remove_cvref_t<T>>::value == 2U>>> =
        is_tuple_like_v<T>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_TUPLE_LIKE_HPP_
