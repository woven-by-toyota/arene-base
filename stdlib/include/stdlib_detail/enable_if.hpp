// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ENABLE_IF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ENABLE_IF_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {
/// @brief Class to support constraining templates. Has a @c type member of the specified type if the @c Condition is @c
/// true and no @c type member if the @c Condition is @c false
/// @tparam Condition the condition to check
/// @tparam Type the type of the member
template <bool Condition, typename Type = void>
class enable_if {};

/// @brief Specialization of a class to support constraining templates for the case that the condition is @c true which
/// has a @c type member of the specified type
/// @tparam Type the type of the member
template <typename Type>
class enable_if<true, Type> {
 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-e "False positive: type is not hiding any identifier"
  /// @brief The result type
  using type = Type;
  // parasoft-end-suppress AUTOSAR-A2_10_1-e "False positive: type is not hiding any identifier"
};

// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: There is an @brief tag"
/// @brief Type alias to support constraining templates. Is an alias for the specified type if the @c Condition is @c
/// true and is not defined if the @c Condition is @c false
/// @tparam Condition the condition to check
/// @tparam Type the type of the result
template <bool Condition, typename Type = void>
// parasoft-begin-suppress AUTOSAR-A2_10_1-e-2 "False positive: enable_if_t is not hiding any identifier"
using enable_if_t = typename enable_if<Condition, Type>::type;
// parasoft-end-suppress AUTOSAR-A2_10_1-e-2
// parasoft-end-suppress AUTOSAR-A2_7_3-a-2

}  // namespace std
#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ENABLE_IF_HPP_
