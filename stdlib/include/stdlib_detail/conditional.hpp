// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_CONDITIONAL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_CONDITIONAL_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>

namespace std {

/// @brief Helper template for selecting types based on a condition. If the supplied @c Condition is @c true then the
/// nested @c type member is an alias for @c TrueType otherwise it is an alias for @c FalseType
/// @tparam Condition The condition
/// @tparam TrueType The type to use if the condition is @c true
/// @tparam FalseType The type to use if the condition is @c false
template <bool Condition, typename TrueType, typename FalseType>
class conditional {
 public:
  /// @brief The selected type
  using type = FalseType;
};

// parasoft-begin-suppress AUTOSAR-A14_7_2-a-2 "False positive: All
// 'conditional' specializations are declared in the same files as the
// primary template"

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: all declarations
// and typedefs *are* preceded by a comment with @brief"

/// @brief Helper template for selecting types based on a condition. If the supplied @c Condition is @c true then the
/// nested @c type member is an alias for @c TrueType otherwise it is an alias for @c FalseType
///
/// This specialization handles the case that the condition is @c true
/// @tparam TrueType The type to use if the condition is @c true
/// @tparam FalseType The type to use if the condition is @c false
template <typename TrueType, typename FalseType>
class conditional<true, TrueType, FalseType> {
 public:
  /// @brief The selected type
  using type = TrueType;
};

// parasoft-end-suppress AUTOSAR-A2_7_3-a
// parasoft-end-suppress AUTOSAR-A14_7_2-a-2

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: the typedef *is* preceded by a comment with @brief"
/// @brief Helper template alias for selecting types based on a condition. If the supplied @c Condition is @c true then
/// it is an alias for @c TrueType otherwise it is an alias for @c FalseType
/// @tparam Condition The condition
/// @tparam TrueType The type to use if the condition is @c true
/// @tparam FalseType The type to use if the condition is @c false
template <bool Condition, typename TrueType, typename FalseType>
using conditional_t = typename conditional<Condition, TrueType, FalseType>::type;
// parasoft-end-suppress AUTOSAR-A2_7_3-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_CONDITIONAL_HPP_
