// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_EXTENT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_EXTENT_HPP_

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"

namespace std {

// parasoft-begin-suppress AUTOSAR-A3_9_1-b "The use of 'unsigned' is specified in the standard"

/// @brief obtains the size of an array type along a specified dimension
/// @tparam T type to query
/// @tparam N specified array dimension
///
/// If @c T is an array type of rank greater than @c N, obtains the bound of the
/// @c N 'th dimension of @c T as the value. If @c T is an array type of unknown
/// bound, then the value is 0. In all other cases, value is 0.
///
template <class T, unsigned N = 0>
extern constexpr auto extent_v = size_t{};

// NOLINTBEGIN(hicpp-avoid-c-arrays)

/// @brief obtains the size of an array type along a specified dimension
/// @tparam T type to query
///
/// If @c T is an array type of rank greater than @c N, obtains the bound of the
/// @c N 'th dimension of @c T as the value. If @c T is an array type of unknown
/// bound, then the value is 0. In all other cases, value is 0.
///
template <class T>
extern constexpr auto extent_v<T[], 0> = size_t{};

/// @brief obtains the size of an array type along a specified dimension
/// @tparam T type to query
/// @tparam N specified array dimension
///
/// If @c T is an array type of rank greater than @c N, obtains the bound of the
/// @c N 'th dimension of @c T as the value. If @c T is an array type of unknown
/// bound, then the value is 0. In all other cases, value is 0.
///
template <class T, unsigned N>
extern constexpr auto extent_v<T[], N> = extent_v<T, N - 1>;

/// @brief obtains the size of an array type along a specified dimension
/// @tparam T type to query
/// @tparam I array bound at dimension 0
///
/// If @c T is an array type of rank greater than @c N, obtains the bound of the
/// @c N 'th dimension of @c T as the value. If @c T is an array type of unknown
/// bound, then the value is 0. In all other cases, value is 0.
///
template <class T, size_t I>
extern constexpr auto extent_v<T[I], 0> = size_t{I};

/// @brief obtains the size of an array type along a specified dimension
/// @tparam T type to query
/// @tparam I array bound at dimension 0
/// @tparam N specified array dimension
///
/// If @c T is an array type of rank greater than @c N, obtains the bound of the
/// @c N 'th dimension of @c T as the value. If @c T is an array type of unknown
/// bound, then the value is 0. In all other cases, value is 0.
///
template <class T, size_t I, unsigned N>
extern constexpr auto extent_v<T[I], N> = extent_v<T, N - 1>;

// NOLINTEND(hicpp-avoid-c-arrays)

/// @brief obtains the size of an array type along a specified dimension
/// @tparam T type to query
/// @tparam N specified array dimension
///
/// If @c T is an array type of rank greater than @c N, obtains the bound of the
/// @c N 'th dimension of @c T as the value. If @c T is an array type of unknown
/// bound, then the value is 0. In all other cases, value is 0.
///
template <class T, unsigned N = 0>
class extent : public integral_constant<size_t, extent_v<T, N>> {};

// parasoft-end-suppress AUTOSAR-A3_9_1-b

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_EXTENT_HPP_
