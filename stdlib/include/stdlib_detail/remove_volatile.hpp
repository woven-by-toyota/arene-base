// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_VOLATILE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_VOLATILE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {
/// @brief remove @c volatile from the provided type. The @c type member is an alias for the modified type.
/// @tparam Type The type to remove @c volatile to
template <typename Type>
class remove_volatile {
 public:
  /// @brief The resulting type
  using type = Type;
};

// parasoft-begin-suppress AUTOSAR-A2_11_1-a-2 "volatile used to remove volatile qualifier"
/// @brief remove @c volatile from the provided type. The @c type member is an alias for the modified type.
/// @tparam Type The type to remove @c volatile to
template <typename Type>
class remove_volatile<Type volatile> {
 public:
  /// @brief The resulting type
  using type = Type;
};
// parasoft-end-suppress AUTOSAR-A2_11_1-a-2

/// @brief remove @c volatile from the provided type. An alias for the modified type.
/// @tparam Type The type to remove @c volatile to
template <typename Type>
using remove_volatile_t = typename remove_volatile<Type>::type;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_VOLATILE_HPP_
