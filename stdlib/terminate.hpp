// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_TERMINATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_TERMINATE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: friend "stdlib/.*"

namespace std {

/// @brief function called when exception handling fails
///
/// @note This is an incomplete implementation of @c std::terminate that is only
/// enough to allow use of @c //arene/base:contracts within @c //stdlib.
///
[[noreturn]] void terminate() noexcept;

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_TERMINATE_HPP_
