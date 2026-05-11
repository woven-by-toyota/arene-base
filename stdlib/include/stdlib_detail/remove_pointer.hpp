// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_POINTER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_POINTER_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: all declarations
// and typedefs *are* preceded by a comment with @brief"

// parasoft-begin-suppress AUTOSAR-A14_7_2-a-2 "False positive: All
// 'remove_pointer' specializations are declared in the same files as the
// primary template"

/// @brief The @c type member is an alias for the type pointed to by @c T, or the same as @c T if it is not a pointer.
/// @tparam T The type to remove the pointer from
template <typename T>
class remove_pointer {
 public:
  /// @brief The resulting type
  using type = T;
};

/// @brief The @c type member is an alias for the type pointed to by @c T, or the same as @c T if it is not a pointer.
/// @tparam T The type to remove the pointer from
template <typename T>
class remove_pointer<T*> {
 public:
  /// @brief The resulting type
  using type = T;
};

/// @brief The @c type member is an alias for the type pointed to by @c T, or the same as @c T if it is not a pointer.
/// @tparam T The type to remove the pointer from
template <typename T>
class remove_pointer<T* const> {
 public:
  /// @brief The resulting type
  using type = T;
};

// parasoft-begin-suppress AUTOSAR-A2_11_1-a-2 "volatile used to remove volatile qualifier"
/// @brief The @c type member is an alias for the type pointed to by @c T, or the same as @c T if it is not a pointer.
/// @tparam T The type to remove the pointer from
template <typename T>
class remove_pointer<T* volatile> {
 public:
  /// @brief The resulting type
  using type = T;
};

/// @brief The @c type member is an alias for the type pointed to by @c T, or the same as @c T if it is not a pointer.
/// @tparam T The type to remove the pointer from
template <typename T>
class remove_pointer<T* const volatile> {
 public:
  /// @brief The resulting type
  using type = T;
};
// parasoft-end-suppress AUTOSAR-A2_11_1-a-2

/// @brief The @c type member is an alias for the type pointed to by @c T, or the same as @c T if it is not a pointer.
/// @tparam T The type to remove the pointer from
template <typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

// parasoft-end-suppress AUTOSAR-A2_7_3-a
// parasoft-end-suppress AUTOSAR-A14_7_2-a-2

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_REMOVE_POINTER_HPP_
