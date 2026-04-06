// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_ELEMENT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_ELEMENT_HPP_

// IWYU pragma: private, include <tuple>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "stdlib/include/stdlib_detail/add_const.hpp"
#include "stdlib/include/stdlib_detail/add_cv.hpp"
#include "stdlib/include/stdlib_detail/add_volatile.hpp"
#include "stdlib/include/stdlib_detail/cstddef.hpp"

namespace std {

/// @brief obtains the type of the specified element
/// @tparam I index to obtain the element type of
/// @tparam T @c tuple-like type
///
/// Provides compile-time indexed access to the types of the elements of a @c tuple-like type.
///
template <size_t I, class T>
class tuple_element;  // undefined

/// @brief obtains the type of the specified element
/// @tparam I index to obtain the element type of
/// @tparam T @c tuple-like type
///
/// Specialization for @c const qualified @c T.
///
template <size_t I, class T>
class tuple_element<I, T const> : public add_const<typename tuple_element<I, T>::type> {};

// parasoft-begin-suppress AUTOSAR-A2_11_1-a "This type trait is required to add volatile"

/// @brief obtains the type of the specified element
/// @tparam I index to obtain the element type of
/// @tparam T @c tuple-like type
///
/// Specialization for @c volatile qualified @c T.
///
template <size_t I, class T>
class tuple_element<I, T volatile> : public add_volatile<typename tuple_element<I, T>::type> {};

/// @brief obtains the type of the specified element
/// @tparam I index to obtain the element type of
/// @tparam T @c tuple-like type
///
/// Specialization for @c const and @c volatile qualified @c T.
///
template <size_t I, class T>
class tuple_element<I, T const volatile> : public add_cv<typename tuple_element<I, T>::type> {};

// parasoft-end-suppress AUTOSAR-A2_11_1-a

/// @brief obtains the type of the specified element
/// @tparam I index to obtain the element type of
/// @tparam T @c tuple-like type
///
template <size_t I, class T>
using tuple_element_t = typename tuple_element<I, T>::type;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_ELEMENT_HPP_
