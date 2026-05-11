// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADD_CV_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADD_CV_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {
/// @brief Add @c const and @c volatile to the provided type. The @c type member
/// is an alias for the modified type.
/// @tparam Type The type to add @c const and @c volatile to
template <typename Type>
class add_cv {
 public:
  // parasoft-begin-suppress AUTOSAR-A2_11_1-a "This type trait is required to add volatile"
  /// @brief The resulting type
  using type = Type const volatile;
  // parasoft-end-suppress AUTOSAR-A2_11_1-a
};

/// @brief Add @c const and @c volatile to the provided type. An alias for the
/// modified type.
/// @tparam Type The type to add @c const and @c volatile to
template <typename Type>
using add_cv_t = typename add_cv<Type>::type;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ADD_CV_HPP_
