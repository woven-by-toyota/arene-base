// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_CONST_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_CONST_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {
/// @brief remove @c const from the provided type. The @c type member is an alias for the modified type.
/// @tparam Type The type to remove @c const to
template <typename Type>
class remove_const {
 public:
  /// @brief The resulting type
  using type = Type;
};

/// @brief remove @c const from the provided type. The @c type member is an alias for the modified type.
/// @tparam Type The type to remove @c const to
template <typename Type>
class remove_const<Type const> {
 public:
  /// @brief The resulting type
  using type = Type;
};

/// @brief remove @c const from the provided type. An alias for the modified type.
/// @tparam Type The type to remove @c const to
template <typename Type>
using remove_const_t = typename remove_const<Type>::type;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_CONST_HPP_
