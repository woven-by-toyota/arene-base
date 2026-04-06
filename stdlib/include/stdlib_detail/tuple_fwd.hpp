// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_FWD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_FWD_HPP_

// IWYU pragma: private, include <tuple>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "stdlib/include/stdlib_detail/tuple_element.hpp"

namespace std {

/// @brief a heterogeneous, fixed-size collection of values
/// @tparam Types the types of elements that the tuple stores
template <class... Types>
class tuple;
//
/// @brief access an element of a mutable lvalue reference to a @c tuple
/// @tparam I index of the element to access
/// @tparam Types types of the @c tuple
/// @param tup @c tuple to access an element from
/// @return reference to the @c I'th element of @c tup
template <size_t I, class... Types>
constexpr auto get(tuple<Types...>& tup) noexcept -> tuple_element_t<I, tuple<Types...>>&;

/// @brief access an element of a const lvalue reference to a @c tuple
/// @tparam I index of the element to access
/// @tparam Types types of the @c tuple
/// @param tup @c tuple to access an element from
/// @return reference to the @c I'th element of @c tup
template <size_t I, class... Types>
constexpr auto get(tuple<Types...> const& tup) noexcept -> tuple_element_t<I, tuple<Types...>> const&;

// parasoft-begin-suppress AUTOSAR-A8_4_6 "False positive: This is only the function declaration"
// parasoft-begin-suppress AUTOSAR-A8_4_5 "False positive: This is only the function declaration"
// parasoft-begin-suppress AUTOSAR-A12_8_4 "False positive: This is only the function declaration"
/// @brief access an element of a mutable rvalue reference to a @c tuple
/// @tparam I index of the element to access
/// @tparam Types types of the @c tuple
/// @param tup @c tuple to access an element from
/// @return reference to the @c I'th element of @c tup
template <size_t I, class... Types>
constexpr auto get(tuple<Types...>&& tup) noexcept -> tuple_element_t<I, tuple<Types...>>&&;
// parasoft-end-suppress AUTOSAR-A8_4_6
// parasoft-end-suppress AUTOSAR-A8_4_5
// parasoft-end-suppress AUTOSAR-A12_8_4

/// @brief access an element of a const rvalue reference to a @c tuple
/// @tparam I index of the element to access
/// @tparam Types types of the @c tuple
/// @param tup @c tuple to access an element from
/// @return reference to the @c I'th element of @c tup
template <size_t I, class... Types>
constexpr auto get(tuple<Types...> const&& tup) noexcept -> tuple_element_t<I, tuple<Types...>> const&&;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_FWD_HPP_
