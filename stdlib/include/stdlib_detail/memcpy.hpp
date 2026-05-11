// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MEMCPY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MEMCPY_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"
//
// IWYU pragma: private, include <cstring>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/cstddef.hpp"

namespace std {

// parasoft-begin-suppress AUTOSAR-M17_0_3-a "This is an implementation of the standard library function"
// parasoft-begin-suppress CERT_CPP-DCL51-f "This is an implementation of the standard library function"
// parasoft-begin-suppress AUTOSAR-A7_1_1-a "This definition is required by the standard specification."
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

/// @brief copies one buffer to another
/// @param dest destination buffer
/// @param src source buffer
/// @param count number of bytes to copy
/// @return pointer to the destination buffer
inline auto memcpy(void* dest, void const* src, std::size_t count) -> void* {
  return __builtin_memcpy(dest, src, count);
}

// parasoft-end-suppress AUTOSAR-A7_1_5-a-2
// parasoft-end-suppress AUTOSAR-A7_1_1-a
// parasoft-end-suppress CERT_CPP_DCL51-f
// parasoft-end-suppress AUTOSAR-M17_0_3-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_MEMCPY_HPP_
