// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_CSTDINT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_CSTDINT_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "Part of a standard library implementation"

// IWYU pragma: private, include <cstdint>
// IWYU pragma: friend "stdlib_detail/.*"

// parasoft-begin-suppress AUTOSAR-A18_0_1-a-2 "False positive: This is the implementation of <cstdint>"
// parasoft-begin-suppress AUTOSAR-A1_1_1-d-2 "False positive: This is the implementation of <cstdint>"
// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stdint.h>
// parasoft-end-suppress AUTOSAR-A1_1_1-d-2
// parasoft-end-suppress AUTOSAR-A18_0_1-a-2

namespace std {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: all declarations
// and typedefs *are* preceded by a comment with @brief"

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
/// @brief An 8-bit unsigned integer type
using uint8_t = ::uint8_t;
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
/// @brief A 16-bit unsigned integer type
// NOLINTNEXTLINE(google-runtime-int)
using uint16_t = ::uint16_t;
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
/// @brief A 32-bit unsigned integer type
using uint32_t = ::uint32_t;
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
/// @brief A 64-bit unsigned integer type
// NOLINTNEXTLINE(google-runtime-int,readability-magic-numbers)
using uint64_t = ::uint64_t;
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

/// @brief A maximum-width unsigned integer type
using uintmax_t = ::uintmax_t;

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
/// @brief An 8-bit signed integer type
using int8_t = ::int8_t;
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
/// @brief A 16-bit signed integer type
// NOLINTNEXTLINE(google-runtime-int)
using int16_t = ::int16_t;
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
/// @brief A 32-bit signed integer type
using int32_t = ::int32_t;
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
/// @brief A 64-bit signed integer type
using int64_t = ::int64_t;
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

/// @brief A maximum-width signed integer type
using intmax_t = ::intmax_t;

/// @brief A signed integer type the same size as a pointer
using intptr_t = ::intptr_t;

/// @brief An unsigned integer type the same size as a pointer
using uintptr_t = ::uintptr_t;

// parasoft-end-suppress AUTOSAR-A2_7_3-a

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_CSTDINT_HPP_
