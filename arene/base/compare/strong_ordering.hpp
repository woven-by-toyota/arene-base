// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPARE_STRONG_ORDERING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPARE_STRONG_ORDERING_HPP_

// IWYU pragma: private, include "arene/base/compare.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax pmeritted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_transparent_comparator_for.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A13_5_5-b-2 "Mixed comparisons permitted by A13-5-5 Permit #1"

namespace arene {
namespace base {

namespace strong_ordering_detail {
/// @brief Internal helper class for providing a type that is constructible from literal @c 0, but not from a general
/// integer
class comparison_marker_helper {
  /// @brief Private type inaccessible from elsewhere
  class internal {};

 public:
  /// @brief A class member pointer pointing to a data member of a private type, for which there are no instances.
  /// Constructible from @c 0 or @c nullptr but not from a general integer or pointer.
  using comp_ptr = comparison_marker_helper::internal comparison_marker_helper::*;
};

/// @brief A special type for use as an argument for @c strong_ordering comparison operators to allow comparisons
/// between @c strong_ordering values and literal @c 0, without allowing too many implicit conversions.
using comparison_marker_type = comparison_marker_helper::comp_ptr;

/// @brief The result type of a three-way comparison. Equivalent to @c std::strong_ordering from C++20.
enum class strong_ordering : std::int8_t {
  /// @brief Return value to indicate that the lhs of the comparison was less than the rhs.
  less = -1,
  /// @brief Return value to indicate that the lhs of the comparison was equal to the rhs.
  equal = 0,
  /// @brief Return value to indicate that the lhs of the comparison was greater than the rhs.
  greater = 1
};

/// @brief Comparison for @c strong_ordering against literal 0
/// @param lhs the value being checked
/// @return bool @c true if @c lhs is @c strong_ordering::equal, @c false otherwise.
inline constexpr auto operator==(strong_ordering const lhs, comparison_marker_type) noexcept -> bool {
  return lhs == strong_ordering::equal;
}

/// @brief Comparison for @c strong_ordering against literal 0
/// @param rhs the value being checked
/// @return bool @c true if @c rhs is @c strong_ordering::equal, @c false otherwise.
inline constexpr auto operator==(comparison_marker_type, strong_ordering const rhs) noexcept -> bool {
  return strong_ordering::equal == rhs;
}

/// @brief Comparison for @c strong_ordering against literal 0
/// @param lhs the value being checked
/// @return bool @c true if @c lhs is not @c strong_ordering::equal, @c false otherwise.
inline constexpr auto operator!=(strong_ordering const lhs, comparison_marker_type) noexcept -> bool {
  return lhs != strong_ordering::equal;
}

/// @brief Comparison for @c strong_ordering against literal 0
/// @param rhs the value being checked
/// @return bool @c true if @c rhs is not @c strong_ordering::equal, @c false otherwise.
inline constexpr auto operator!=(comparison_marker_type, strong_ordering const rhs) noexcept -> bool {
  return strong_ordering::equal != rhs;
}

/// @brief Comparison for @c strong_ordering against literal 0
/// @param lhs the value being checked
/// @return bool @c true if @c lhs is @c strong_ordering::less, @c false otherwise.
inline constexpr auto operator<(strong_ordering const lhs, comparison_marker_type) noexcept -> bool {
  // parasoft-begin-suppress AUTOSAR-M0_1_2-e-2 "False positive: lhs could be anything"
  return lhs < strong_ordering::equal;
  // parasoft-end-suppress AUTOSAR-M0_1_2-e-2
}

/// @brief Comparison for @c strong_ordering against literal 0
/// @param rhs the value being checked
/// @return bool @c true if @c rhs is @c strong_ordering::greater, @c false otherwise.
inline constexpr auto operator<(comparison_marker_type, strong_ordering const rhs) noexcept -> bool {
  return strong_ordering::equal < rhs;
}

/// @brief Comparison for @c strong_ordering against literal 0
/// @param lhs the value being checked
/// @return bool @c true if @c lhs is @c strong_ordering::equal or @c strong_ordering::less, @c false otherwise.
inline constexpr auto operator<=(strong_ordering const lhs, comparison_marker_type) noexcept -> bool {
  return lhs <= strong_ordering::equal;
}

/// @brief Comparison for @c strong_ordering against literal 0
/// @param rhs the value being checked
/// @return bool @c true if @c rhs is @c strong_ordering::equal or @c strong_ordering::greater, @c false otherwise.
inline constexpr auto operator<=(comparison_marker_type, strong_ordering const rhs) noexcept -> bool {
  // parasoft-begin-suppress AUTOSAR-M0_1_2-b-2 "False positive: rhs could be anything"
  return strong_ordering::equal <= rhs;
  // parasoft-end-suppress AUTOSAR-M0_1_2-b-2
}

/// @brief Comparison for @c strong_ordering against literal 0
/// @param lhs the value being checked
/// @return bool @c true if @c lhs is @c strong_ordering::greater, @c false otherwise.
inline constexpr auto operator>(strong_ordering const lhs, comparison_marker_type) noexcept -> bool {
  return lhs > strong_ordering::equal;
}

/// @brief Comparison for @c strong_ordering against literal 0
/// @param rhs the value being checked
/// @return bool @c true if @c lhs is @c strong_ordering::greater, @c false otherwise.
inline constexpr auto operator>(comparison_marker_type, strong_ordering const rhs) noexcept -> bool {
  // parasoft-begin-suppress AUTOSAR-M0_1_2-b-2 "False positive: rhs could be anything"
  return strong_ordering::equal > rhs;
  // parasoft-end-suppress AUTOSAR-M0_1_2-b-2
}

/// @brief Comparison for @c strong_ordering against literal 0
/// @param lhs the value being checked
/// @return bool @c true if @c lhs is @c strong_ordering::equal or @c strong_ordering::greater, @c false otherwise.
inline constexpr auto operator>=(strong_ordering const lhs, comparison_marker_type) noexcept -> bool {
  // parasoft-begin-suppress AUTOSAR-M0_1_2-e-2 "False positive: lhs could be anything"
  return lhs >= strong_ordering::equal;
  // parasoft-end-suppress AUTOSAR-M0_1_2-e-2
}

/// @brief Comparison for @c strong_ordering against literal 0
/// @param rhs the value being checked
/// @return bool @c true if @c rhs is @c strong_ordering::equal or @c strong_ordering::less, @c false otherwise.
inline constexpr auto operator>=(comparison_marker_type, strong_ordering const rhs) noexcept -> bool {
  return strong_ordering::equal >= rhs;
}
}  // namespace strong_ordering_detail

/// @brief The enumeration specifying the result of an ordering comparison
using strong_ordering = strong_ordering_detail::strong_ordering;

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "Conversion of strong_ordering to and from 'int' is deliberate choice, for
// compatibility with existing APIs that produce or expect an 'int' result"

/// @brief Convert an integer three-way-comparison result into a @c strong_ordering.
/// @param three_way_result The result to convert.
/// @return strong_ordering @c strong_ordering::less if @c three_way_result is less than zero, @c strong_ordering::equal
/// if @c three_way_result is equal to zero, and @c strong_ordering::greater if @c three_way_result is greater than
/// zero.
inline constexpr auto make_strong_ordering(int const three_way_result) noexcept -> strong_ordering {
  if (three_way_result < 0) {
    return strong_ordering::less;
  }
  if (three_way_result == 0) {
    return strong_ordering::equal;
  }
  return strong_ordering::greater;
}

// parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "ARENE_UNREACHABLE terminates if it is ever reached, so does not need a
// return"
// parasoft-begin-suppress CERT_C-MSC37-a-2 "ARENE_UNREACHABLE terminates if it is ever reached, so does not need a
// return"
// parasoft-begin-suppress CERT_CPP-MSC52-a-2 "ARENE_UNREACHABLE terminates if it is ever reached, so does not need a
// return"
/// @brief Convert a @c strong_ordering into an integer three-way-comparison result.
/// @param ordering The ordering to convert.
/// @return int @c -1 if @c ordering is strong_ordering::less, @c 0 if @c ordering is strong_ordering::equal, and @c 1
/// if @c ordering is strong_ordering::greater.
inline constexpr auto from_strong_ordering(strong_ordering const ordering) noexcept -> int {
  // parasoft-begin-suppress AUTOSAR-M6_4_5-a-2 "All branches terminated with return as per permit M6-4-5#1"
  // parasoft-begin-suppress AUTOSAR-M6_4_3-a-2 "All branches terminated with return as per permit M6-4-3#1"
  switch (ordering) {
    case strong_ordering::less:
      return -1;
    case strong_ordering::equal:
      return 0;
    case strong_ordering::greater:
      return 1;
  }
  // parasoft-end-suppress AUTOSAR-M6_4_5-a-2
  // parasoft-end-suppress AUTOSAR-M6_4_3-a-2
  ARENE_INVARIANT_UNREACHABLE("Invalid ordering");
}
// parasoft-end-suppress CERT_CPP-MSC52-a-2
// parasoft-end-suppress CERT_C-MSC37-a-2
// parasoft-end-suppress AUTOSAR-A8-4-2-a-2
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A8_4_2-a-2 "ARENE_UNREACHABLE to terminate if it is ever reached, so does not need a
// return"
// parasoft-begin-suppress CERT_C-MSC37-a-2 "ARENE_UNREACHABLE terminates if it is ever reached, so does not need a
// return"
// parasoft-begin-suppress CERT_CPP-MSC52-a-2 "ARENE_UNREACHABLE terminates if it is ever reached, so does not need a
// return"
/// @brief Return the opposite ordering for a @c strong_ordering three-way-comparison
/// result.
/// @param ordering The ordering to convert.
/// @return strong_ordering @c strong_ordering::greater if @c ordering is strong_ordering::less, @c
/// strong_ordering::equal if @c ordering is strong_ordering::equal, and @c strong_ordering::less if @c ordering is
/// strong_ordering::greater.
inline constexpr auto opposite_ordering(strong_ordering const ordering) noexcept -> strong_ordering {
  // parasoft-begin-suppress AUTOSAR-M6_4_5-a-2 "All branches terminated with return as per permit M6-4-5#1"
  // parasoft-begin-suppress AUTOSAR-M6_4_3-a-2 "All branches terminated with return as per permit M6-4-3#1"
  switch (ordering) {
    case strong_ordering::less:
      return strong_ordering::greater;
    case strong_ordering::greater:
      return strong_ordering::less;
    case strong_ordering::equal:
      return strong_ordering::equal;
  }
  // parasoft-end-suppress AUTOSAR-M6_4_5-a-2
  // parasoft-end-suppress AUTOSAR-M6_4_3-a-2
  ARENE_INVARIANT_UNREACHABLE("Invalid ordering");
}
// parasoft-end-suppress CERT_CPP-MSC52-a-2
// parasoft-end-suppress CERT_C-MSC37-a-2
// parasoft-end-suppress AUTOSAR-A8-4-2-a-2

/// @brief Trait to determine if a given type is a _transparent comparator_ that performs a three-way comparison for a
/// given set of operands.
///
/// @tparam C The type to test
/// @tparam T The type of the left hand operand
/// @tparam U The type of the right hand operand
/// @return bool Will be equivalent to <c>is_transparent_comparator_v<C> && is_invocable_r_v<strong_ordering, C, T,
/// U></c>.

template <typename C, typename T, typename U = T>
extern constexpr bool is_transparent_three_way_comparator_for_v =
    is_transparent_comparator_v<C> && is_invocable_r_v<strong_ordering, C, T, U>;

/// @brief Trait to determine if a given type is a _transparent comparator_ for a given set of operands.
///
/// @tparam C The type to test
/// @tparam T The type of the left hand operand
/// @tparam U The type of the right hand operand
/// Value will be equivalent to <c>is_transparent_three_way_comparator_v<C> && is_invocable_r_v<strong_ordering, C,
/// T, U></c>.
template <typename C, typename T, typename U = T>
using is_transparent_three_way_comparator_for =
    std::integral_constant<bool, is_transparent_three_way_comparator_for_v<C, T, U>>;

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A13_5_5-b-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPARE_STRONG_ORDERING_HPP_
