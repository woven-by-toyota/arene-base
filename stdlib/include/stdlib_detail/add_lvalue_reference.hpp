// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADD_LVALUE_REFERENCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADD_LVALUE_REFERENCE_HPP_

#include "arene/base/constraints.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: all declarations
// and typedefs *are* preceded by a comment with @brief"

namespace add_lvalue_reference_detail {

/// @brief Base case when @c Type is not referenceable.
/// @tparam Type The type to add an lvalue reference to
template <typename Type, class = arene::base::constraints<>>
class add_lvalue_reference {
 public:
  /// @brief The resulting type
  using type = Type;
};

/// @brief Add an lvalue reference onto @c Type
/// @tparam Type The type to add an lvalue reference to
template <typename Type>
class add_lvalue_reference<Type, arene::base::constraints<Type&>> {
 public:
  /// @brief The resulting type
  using type = Type&;
};

}  // namespace add_lvalue_reference_detail

/// @brief Add an lvalue reference onto @c Type
/// @tparam Type The type to add an lvalue reference to
///
/// @c add_lvalue_reference works with both referenceable and non-referenceable types. Referenceable types are types in
/// one of three categories:
///   1. object types
///   2. function types that are not cv-ref qualified.
///   3. reference types
///
/// For category 3, this should follow reference collapsing rules.
///
/// Any type not in one of these three categories is a "non-referenceable" type. For example, void.
/// For non-referenceable types, @c add_lvalue_reference will return back the input type.
///
template <typename Type>
using add_lvalue_reference = add_lvalue_reference_detail::add_lvalue_reference<Type>;

/// @brief Add an lvalue reference onto @c Type
/// @tparam Type The type to add an lvalue reference to
template <typename Type>
using add_lvalue_reference_t = typename add_lvalue_reference<Type>::type;

// parasoft-end-suppress AUTOSAR-A2_7_3-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADD_LVALUE_REFERENCE_HPP_
