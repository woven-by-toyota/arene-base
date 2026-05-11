// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_SIZE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_SIZE_HPP_

// IWYU pragma: private, include <tuple>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "stdlib/include/stdlib_detail/cstddef.hpp"

namespace std {

/// @brief obtain the number of elements in a tuple-like type
/// @tparam T tuple like type
///
template <class T>
class tuple_size;  // undefined

/// @brief obtain the number of elements in a tuple-like type
/// @tparam T tuple like type
///
/// Specialization for @c const qualified @c T.
///
template <class T>
class tuple_size<T const> : public tuple_size<T> {};

// parasoft-begin-suppress AUTOSAR-A2_11_1-a "This type trait is required to add volatile"

/// @brief obtain the number of elements in a tuple-like type
/// @tparam T tuple like type
///
/// Specialization for @c volatile qualified @c T.
///
template <class T>
class tuple_size<T volatile> : public tuple_size<T> {};

/// @brief obtain the number of elements in a tuple-like type
/// @tparam T tuple like type
///
/// Specialization for @c const and @c volatile qualified @c T.
///
template <class T>
class tuple_size<T const volatile> : public tuple_size<T> {};

// parasoft-end-suppress AUTOSAR-A2_11_1-a

/// @brief obtain the number of elements in a tuple-like type
/// @tparam T tuple like type
///
// NOTE: While most type traits in this 'std' implementation use _v as the source of truth,
// @c tuple_size_v is based on @c tuple_size as types (including user-defined types) are
// expected to provide specializations of @c tuple_size.
template <class T>
extern constexpr std::size_t tuple_size_v{tuple_size<T>::value};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_TUPLE_SIZE_HPP_
