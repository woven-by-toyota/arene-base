// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NEW_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NEW_HPP_

// IWYU pragma: private, include <new>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-M7_3_1-a "Part of a standard library implementation"

#include "stdlib/include/stdlib_detail/cstddef.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_2-b "This definition is required by the standard specification."
/// @brief Implementation of placement new; does no dynamic allocation. This is called for all placement new expressions
/// of the form <c>new (ptr) Type</c> where @c ptr is convertible to @c void* and no other overload is viable.
///
/// This is not a user-replaceable function
/// @param ptr The address to construct at
/// @return The supplied @c ptr
inline auto operator new(std::size_t, void* const ptr) noexcept -> void* { return ptr; }
// parasoft-end-suppress AUTOSAR-A7_1_2-b

// parasoft-begin-suppress AUTOSAR-A7_1_2-b "This definition is required by the standard specification."
/// @brief Implementation of placement new for arrays; does no dynamic allocation. This is called for all placement new
/// expressions of the form <c>new (ptr) Type[count]</c> where @c ptr is convertible to @c void* and no other overload
/// is viable.
///
/// This is not a user-replaceable function
/// @param ptr The address to construct at
/// @return The supplied @c ptr
inline auto operator new[](std::size_t, void* const ptr) noexcept -> void* { return ptr; }
// parasoft-end-suppress AUTOSAR-A7_1_2-b

// parasoft-begin-suppress AUTOSAR-M0_1_8-b "This definition is required by the standard specification."
// parasoft-begin-suppress AUTOSAR-A18_5_4-a "This definition is required by the standard specification."
/// @brief Implementation of placement delete. Used if the constructor called by a placement new expression throws.
///
/// This is not a user-replaceable function
inline void operator delete(void*, void*) noexcept {}
// parasoft-end-suppress AUTOSAR-A18_5_4-a
// parasoft-end-suppress AUTOSAR-M0_1_8-b

// parasoft-begin-suppress AUTOSAR-M0_1_8-b "This definition is required by the standard specification."
// parasoft-begin-suppress AUTOSAR-A18_5_4-a "This definition is required by the standard specification."
/// @brief Implementation of placement delete for arrays. Used if the constructor called by an array placement new
/// expression throws.
///
/// This is not a user-replaceable function
inline void operator delete[](void*, void*) noexcept {}
// parasoft-end-suppress AUTOSAR-A18_5_4-a
// parasoft-end-suppress AUTOSAR-M0_1_8-b

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_NEW_HPP_
