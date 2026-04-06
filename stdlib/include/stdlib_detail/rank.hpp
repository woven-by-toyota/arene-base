// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_RANK_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_RANK_HPP_

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"

namespace std {

/// @brief obtains the number of dimensions of an array type
/// @tparam T type to query
///
/// If @c T is an array type, provides the member constant @c value equal to
/// the number of dimensions of the array. For any other type, @c value is 0.
///
template <class T>
extern constexpr auto rank_v = size_t{};

// NOLINTBEGIN(hicpp-avoid-c-arrays)

/// @brief obtains the number of dimensions of an array type
/// @tparam T type to query
///
/// If @c T is an array type, provides the member constant @c value equal to
/// the number of dimensions of the array. For any other type, @c value is 0.
///
template <class T>
extern constexpr auto rank_v<T[]> = rank_v<T> + 1;

/// @brief obtains the number of dimensions of an array type
/// @tparam T type to query
/// @tparam N array bound
///
/// If @c T is an array type, provides the member constant @c value equal to
/// the number of dimensions of the array. For any other type, @c value is 0.
///
template <class T, size_t N>
extern constexpr auto rank_v<T[N]> = rank_v<T> + 1;

// NOLINTEND(hicpp-avoid-c-arrays)

/// @brief obtains the number of dimensions of an array type
/// @tparam T type to query
///
/// If @c T is an array type, provides the member constant @c value equal to
/// the number of dimensions of the array. For any other type, @c value is 0.
///
template <class T>
class rank : public integral_constant<size_t, rank_v<T>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_RANK_HPP_
