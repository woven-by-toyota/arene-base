// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_EXPECT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_EXPECT_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace expect_detail {
/// @brief The type of the return from @c check
// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "__builtin_expect requires a long parameter"
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented above"
// NOLINTNEXTLINE(google-runtime-int)
using check_return = long;
// parasoft-end-suppress AUTOSAR-A2_7_3
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

/// @brief Simple wrapper function that returns its argument cast to @c long, for use in contract checks as an argument
/// to @c __builtin_expect
/// @tparam T The type of the argument
/// @param arg The value to return
/// @return @c arg cast to @c bool and then cast to @c long
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented above"
template <typename T>
inline constexpr auto check(T&& arg) noexcept -> check_return {
  return static_cast<check_return>(static_cast<bool>(std::forward<T>(arg)));
}
// parasoft-end-suppress AUTOSAR-A2_7_3
}  // namespace expect_detail
}  // namespace base
}  // namespace arene

// parasoft-begin-suppress AUTOSAR-A16_0_1-d-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"

/// @brief A macro to use in an @c if statement to declare that the expression is expected to be @c true more often than
/// it is @c false.
///
/// Usage: @c if(ARENE_EXPECT(the_answer == 42)) { ... }
///
/// @param ... The expression to evaluate
/// @pre expression must be convertible to @c bool

// parasoft-begin-suppress AUTOSAR-M16_0_7-a-2 "False positive: not used as a macro"
#if ARENE_HAS_BUILTIN(__builtin_expect)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_EXPECT(...) (__builtin_expect(::arene::base::expect_detail::check((__VA_ARGS__)), 1) != 0)
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_EXPECT(...) (::arene::base::expect_detail::check((__VA_ARGS__)) != 0)
#endif
// parasoft-end-suppress AUTOSAR-M16_0_7-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_EXPECT_HPP_
