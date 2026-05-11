// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_ALL_EXTENTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_ALL_EXTENTS_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/cstddef.hpp"

namespace std {
/// @brief Removes all extents from the given array type
/// @tparam T The array type to remove all extents from
///
/// If @c T is a (multidimensional) array of some type @c X, provides static
/// member @c type as an alias to @c X. Otherwise if @c T is not an array, @c
/// type is an alias to @c T.
///
template <typename T>
class remove_all_extents {
 public:
  /// @brief The resulting type
  using type = T;
};

// NOLINTBEGIN(hicpp-avoid-c-arrays)

/// @brief Removes all extents from the given array type
/// @tparam T The array type to remove all extents from
///
/// Specialization for array of unknown bound.
///
template <typename T>
class remove_all_extents<T[]> {
 public:
  /// @brief The resulting type
  using type = typename remove_all_extents<T>::type;
};

/// @brief Removes all extents from the given array type
/// @tparam T The array type to remove all extents from
/// @tparam N array size
///
/// Specialization for array of known bound.
///
template <typename T, size_t N>
class remove_all_extents<T[N]> {
 public:
  /// @brief The resulting type
  using type = typename remove_all_extents<T>::type;
};

// NOLINTEND(hicpp-avoid-c-arrays)

// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: the typedef *is* preceded by a comment with @brief"
/// @brief Removes all extents from the given array type
/// @tparam T The array type to remove the extents from
///
/// Alias to @c remove_all_extents<T>::type.
///
template <typename T>
using remove_all_extents_t = typename remove_all_extents<T>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3-a-2

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_ALL_EXTENTS_HPP_
