// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::lexicographic_string_compare"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_LEXICOGRAPHIC_STRING_COMPARE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_LEXICOGRAPHIC_STRING_COMPARE_HPP_

// IWYU pragma: private, include "arene/base/string_algorithms.hpp"
// IWYU pragma: friend "arene/base/strings/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/span/span.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace detail {

/// @brief A type alias for an unsigned character type, to ensure correct UTF-8 ordering
using utf8_compatible_comparison_type = std::make_unsigned_t<character>;

/// @brief Compare two strings lexicographically. The implementation follows the same logic as @c std::string::compare
///
/// @param lhs The first string.
/// @param rhs The second string.
/// @return strong_ordering::equal if the length of lhs and rhs are equal and all characters compare equal.
/// @return strong_ordering::less if the value of the first character that does not match is lexicographically less in
///          lhs than rhs.
/// @return strong_ordering::less if the length of lhs is less than the length of rhs and all characters in the
///          overlapping subset of rhs match those in lhs.
/// @return strong_ordering::greater if the value of the first character that does not match is lexicographically
/// greater
///          in lhs than rhs.
/// @return strong_ordering::greater if the length of lhs is greater than the length of rhs and all characters in the
///          overlapping subset of lhs match those in rhs.
/// @pre If either lhs or rhs is a <c> const char* </c>, they must not be nullptr or else @c ARENE_PRECONDITION
///      violation
/// @pre if either lhs or rhs is a <c> const char* </c>, they must point to a valid null terminated string or else
///      behavior is undefined.
// parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: There is no function that throws an excpetion of 'Any'
// type"
inline constexpr auto lexicographic_string_compare(
    span<character const> const lhs,
    span<character const> const rhs
) noexcept -> strong_ordering {
  auto lhs_iter = lhs.begin();
  auto const lhs_end = lhs.end();
  auto rhs_iter = rhs.begin();
  auto const rhs_end = rhs.end();
  auto const lhs_distance = lhs_end - lhs_iter;
  auto const rhs_distance = rhs_end - rhs_iter;
  for (auto remaining = std::min(lhs_distance, rhs_distance); remaining != 0; --remaining) {
    // Note: we do implement our own loop here due to
    // char_traits not being constexpr in our current version of arm clang
    auto const lchar = static_cast<utf8_compatible_comparison_type>(*lhs_iter);
    auto const rchar = static_cast<utf8_compatible_comparison_type>(*rhs_iter);
    if (lchar < rchar) {
      return strong_ordering::less;
    }
    if (rchar < lchar) {
      return strong_ordering::greater;
    }
    ++lhs_iter;
    ++rhs_iter;
  }
  return compare_three_way{}(lhs_distance, rhs_distance);
}
// parasoft-end-suppress AUTOSAR-A15_4_5-a

/// @brief Compare two strings lexicographically. The implementation follows the same logic as @c std::string::compare
///
/// @param lhs The first string.
/// @param rhs The second string.
/// @return strong_ordering::equal if the length of lhs and rhs are equal and all characters compare equal.
/// @return strong_ordering::less if the value of the first character that does not match is lexicographically less in
///          lhs than rhs.
/// @return strong_ordering::less if the length of lhs is less than the length of rhs and all characters in the
///          overlapping subset of rhs match those in lhs.
/// @return strong_ordering::greater if the value of the first character that does not match is lexicographically
/// greater
///          in lhs than rhs.
/// @return strong_ordering::greater if the length of lhs is greater than the length of rhs and all characters in the
///          overlapping subset of lhs match those in rhs.
/// @pre @c rhs not be nullptr or else @c ARENE_PRECONDITION violation
/// @pre @c rhs must point to a valid null terminated string or else behavior is undefined.
// parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: There is no function that throws an excpetion of 'Any'
// type"
inline constexpr auto lexicographic_string_compare(span<character const> lhs, raw_c_string rhs) noexcept
    -> strong_ordering {
  ARENE_PRECONDITION(rhs != nullptr);
  // Note: We implement our own loop here due to char_traits not being constexpr in our current version of arm clang
  for (character const lhs_char : lhs) {
    character const rhs_char{*rhs};
    arene::base::advance(rhs, 1);
    if (rhs_char == '\0') {
      return strong_ordering::greater;
    }
    if (lhs_char != rhs_char) {
      utf8_compatible_comparison_type const lhs_uchar{static_cast<utf8_compatible_comparison_type>(lhs_char)};
      utf8_compatible_comparison_type const rhs_uchar{static_cast<utf8_compatible_comparison_type>(rhs_char)};
      return (lhs_uchar < rhs_uchar) ? strong_ordering::less : strong_ordering::greater;
    }
  }

  return (*rhs == '\0') ? strong_ordering::equal : strong_ordering::less;
}
// parasoft-end-suppress AUTOSAR-A15_4_5-a

/// @brief Compare two strings lexicographically. The implementation follows the same logic as @c std::string::compare
///
/// @param lhs The first string.
/// @param rhs The second string.
/// @return strong_ordering::equal if the length of lhs and rhs are equal and all characters compare equal.
/// @return strong_ordering::less if the value of the first character that does not match is lexicographically less in
///          lhs than rhs.
/// @return strong_ordering::less if the length of lhs is less than the length of rhs and all characters in the
///          overlapping subset of rhs match those in lhs.
/// @return strong_ordering::greater if the value of the first character that does not match is lexicographically
/// greater
///          in lhs than rhs.
/// @return strong_ordering::greater if the length of lhs is greater than the length of rhs and all characters in the
///          overlapping subset of lhs match those in rhs.
/// @pre @c lhs and @c rhs must not be nullptr or else @c ARENE_PRECONDITION violation
/// @pre @c lhs and @c rhs must point to a valid null terminated string or else behavior is undefined.
// parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: There is no function that throws an excpetion of 'Any'
// type"
inline constexpr auto lexicographic_string_compare(raw_c_string lhs, raw_c_string rhs) noexcept -> strong_ordering {
  ARENE_PRECONDITION(lhs != nullptr);
  ARENE_PRECONDITION(rhs != nullptr);
  while ((*lhs != '\0') && (*lhs == *rhs)) {
    arene::base::advance(lhs, 1);
    arene::base::advance(rhs, 1);
  }

  utf8_compatible_comparison_type const lhs_char{static_cast<utf8_compatible_comparison_type>(*lhs)};
  utf8_compatible_comparison_type const rhs_char{static_cast<utf8_compatible_comparison_type>(*rhs)};
  if (lhs_char == rhs_char) {
    return strong_ordering::equal;
  }
  return (lhs_char < rhs_char) ? strong_ordering::less : strong_ordering::greater;
}
// parasoft-end-suppress AUTOSAR-A15_4_5-a

}  // namespace detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_LEXICOGRAPHIC_STRING_COMPARE_HPP_
