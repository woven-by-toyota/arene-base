// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_REFERENCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_REFERENCE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {
/// @brief The @c type member is an alias for @c T, but with top-level reference qualifiers removed
/// @tparam T The type to remove the reference qualifiers from
template <typename T>
class remove_reference {
 public:
  /// @brief The resulting type
  using type = T;
};

/// @brief The @c type member is an alias for @c T, but with top-level reference qualifiers removed
/// @tparam T The type to remove the reference qualifiers from
template <typename T>
class remove_reference<T&> {
 public:
  /// @brief The resulting type
  using type = T;
};

/// @brief The @c type member is an alias for @c T, but with top-level reference qualifiers removed
/// @tparam T The type to remove the reference qualifiers from
template <typename T>
class remove_reference<T&&> {
 public:
  /// @brief The resulting type
  using type = T;
};

// parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: 'remove_reference_t' does not hide an identifier in
// 'tuple_leaf'"
/// @brief An alias for @c T, but with top-level reference qualifiers removed
/// @tparam T The type to remove the reference qualifiers from
template <typename T>
using remove_reference_t = typename remove_reference<T>::type;
// parasoft-end-suppress AUTOSAR-A2_10_1-e

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_REFERENCE_HPP_
