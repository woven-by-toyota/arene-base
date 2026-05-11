// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_IS_NOEXCEPT_PART_OF_FUNCTION_TYPE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_IS_NOEXCEPT_PART_OF_FUNCTION_TYPE_HPP_

// IWYU pragma: private, include "arene/base/functional.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

// parasoft-begin-suppress AUTOSAR-A16_0_1-a-2 "Conditional defines permitted by A16-0-1 Permit #2"
// parasoft-begin-suppress AUTOSAR-A16_0_1-d "Conditional defines permitted by A16-0-1 Permit #2"

// Set if noexcept is part of function type based on arene flag
// Allowed define macros per A16-0-1 Permit #2 v1.1.0
#if ARENE_IS_ON(ARENE_STD_NOEXCEPT_FUNCTION_TYPE)
#define ARENE_NOEXCEPT_IS_PART_OF_FUNCTION_TYPE true
#else
#define ARENE_NOEXCEPT_IS_PART_OF_FUNCTION_TYPE false
#endif

// parasoft-end-suppress AUTOSAR-A16_0_1-d
// parasoft-end-suppress AUTOSAR-A16_0_1-a-2

namespace arene {
namespace base {

/// @brief Used to check if the @c noexcept specifier is considered part of
/// a function's type. In C++14 and earlier @c noexcept was not considered
/// as part of a function type. However, in C++17 and later @c noexcept is
/// part of a function type. An example:
///
/// <tt>
/// auto without_noexcept() -> void;
///
/// auto with_noexcept() noexcept -> void;
///
/// bool types_match = std::is_same<decltype(&without_noexcept), decltype(&with_noexcept)>::value;
/// </tt>
///
/// In C++14 and earlier @c types_match will be @c true because both types will
/// be <tt>void (*)()</tt>. In C++17 and later @c types_match will be @c false
/// because the second type will be <tt>void (*)() noexcept</tt>.
///
/// This value is templated on a @c Dummy type in order for it to operate with
/// SFINAE. Use as:
/// <tt>std::enable_if_t<is_noexcept_part_of_function_type_v<T>></tt>
///
/// @tparam Dummy An unused type for SFINAE compatibility
template <typename Dummy>
extern constexpr bool is_noexcept_part_of_function_type_v{ARENE_NOEXCEPT_IS_PART_OF_FUNCTION_TYPE};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_IS_NOEXCEPT_PART_OF_FUNCTION_TYPE_HPP_
