// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_STATIC_ASSERTIONS_HELPERS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_STATIC_ASSERTIONS_HELPERS_HPP_

// IWYU pragma: private, include "arene/base/testing/gtest/static_assertions.hpp"
// IWYU pragma: friend "arene/base/testing/gtest/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/forward.hpp"

namespace arene {
namespace base {
namespace testing {
namespace gtest {
namespace detail {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief Simple wrapper function that returns its argument cast to @c bool, for use in assertion checks
/// @tparam T The type of the argument
/// @param arg The value to return
/// @return bool @c arg cast to @c bool
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T>
inline constexpr auto static_assertion_check(T&& arg) noexcept -> bool {
  return static_cast<bool>(std::forward<T>(arg));
}
// parasoft-end-suppress AUTOSAR-A2_7_3
// parasoft-end-suppress AUTOSAR-M3_3_2-a

/// @brief Simple wrapper function for use in assertion checks to detect when a message has accidentally been supplied
/// as a second argument.
/// Always triggers a @c static_assert indicating that the message should not have been supplied
/// @tparam T The type of the argument with the check
/// @tparam MessageLength The length of the supplied message
/// @param arg The value to return
/// @return bool @c arg cast to @c bool
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
template <typename T, std::size_t MessageLength>
// NOLINTNEXTLINE(hicpp-avoid-c-arrays)
inline constexpr auto static_assertion_check(T&& arg, arene::base::detail::character const (&)[MessageLength]) noexcept
    -> bool {
  constexpr bool message_supplied{MessageLength > 0};
  static_assert(!message_supplied, "Message supplied to STATIC_ASSERT macro");
  return static_cast<bool>(std::forward<T>(arg));
}
// parasoft-end-suppress AUTOSAR-A2_7_3

/// @brief Compare two NUL-terminated strings
/// @param lhs The first string
/// @param rhs The second string
/// @return bool @c true iff @c lhs == rhs when interpreted as cstrings, else @c false
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
constexpr auto c_strings_are_equal(
    arene::base::detail::raw_c_string const lhs,
    arene::base::detail::raw_c_string const rhs
) noexcept -> bool {
  arene::base::detail::raw_c_string lhs_pos{lhs};
  arene::base::detail::raw_c_string rhs_pos{rhs};
  while ((*lhs_pos != '\0') && (*lhs_pos == *rhs_pos)) {
    // parasoft-begin-suppress CERT_C-ARR39-b-2 "This function assumes lhs and rhs are NUL-terminated strings"
    // parasoft-begin-suppress AUTOSAR-M5_0_15-a-2 "This function assumes lhs and rhs are NUL-terminated strings"
    ++lhs_pos;
    ++rhs_pos;
    // parasoft-end-suppress AUTOSAR-M5_0_15-a-2
    // parasoft-end-suppress CERT_C-ARR39-b-2
  }
  return (*lhs_pos == '\0') && (*rhs_pos == '\0');
}
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace detail
}  // namespace gtest
}  // namespace testing
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_GTEST_DETAIL_STATIC_ASSERTIONS_HELPERS_HPP_
