// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADD_POINTER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADD_POINTER_HPP_

#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_referenceable.hpp"
#include "stdlib/include/stdlib_detail/is_void.hpp"
#include "stdlib/include/stdlib_detail/remove_reference.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"
namespace std {

namespace add_pointer_detail {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: all declarations
// and typedefs *are* preceded by a comment with @brief"

/// @brief Base case for when @c Type is not referenceable or void
/// @tparam T The type to add the pointer to
template <class Type, class = void>
class add_pointer {
 public:
  /// @brief The resulting type
  using type = Type;
};

/// @brief Add a pointer to the given type
/// @tparam T The type to add the pointer to
template <class Type>
class add_pointer<Type, std::enable_if_t<internal::is_referenceable_v<Type> || std::is_void_v<Type>>> {
 public:
  /// @brief The resulting type
  using type = std::remove_reference_t<Type>*;
};

// parasoft-end-suppress AUTOSAR-A2_7_3-a

}  // namespace add_pointer_detail

/// @brief Adds a pointer to the given type
///
/// If @c Type is a referenceable type or (possibly cv-qualified) void, the member typedef type provided is typename
/// std::remove_reference<T>::type*. Otherwise, the member typedef type provided is T.
/// @tparam T The type to add the pointer to
template <typename Type>
class add_pointer : public add_pointer_detail::add_pointer<Type> {};

/// @brief Adds a pointer to the given type
///
/// If @c Type is a referenceable type or (possibly cv-qualified) void, the member typedef type provided is typename
/// std::remove_reference<T>::type*. Otherwise, the member typedef type provided is T.
/// @tparam T The type to add the pointer to
template <typename Type>
using add_pointer_t = typename add_pointer<Type>::type;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADD_POINTER_HPP_
