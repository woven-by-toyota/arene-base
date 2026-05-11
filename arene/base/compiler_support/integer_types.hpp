// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_INTEGER_TYPES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_INTEGER_TYPES_HPP_

#include "arene/base/stdlib_choice/cstdint.hpp"

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "False positive: uint16_t not reserved in this context"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: uint16_t not reserved in this context"
/// @brief An 8-bit unsigned integer type
using uint8_t = ::std::uint8_t;
// parasoft-end-suppress AUTOSAR-M17_0_2-a-2
// parasoft-end-suppress CERT_CPP-DCL51-f-3
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "False positive: uint16_t not reserved in this context"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: uint16_t not reserved in this context"
/// @brief A 16-bit unsigned integer type
// NOLINTNEXTLINE(google-runtime-int)
using uint16_t = ::std::uint16_t;
// parasoft-end-suppress AUTOSAR-M17_0_2-a-2
// parasoft-end-suppress CERT_CPP-DCL51-f-3
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "False positive: uint32_t not reserved in this context"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: uint32_t not reserved in this context"
/// @brief A 32-bit unsigned integer type
using uint32_t = ::std::uint32_t;
// parasoft-end-suppress AUTOSAR-M17_0_2-a-2
// parasoft-end-suppress CERT_CPP-DCL51-f-3
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "False positive: uint64_t not reserved in this context"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: uint64_t not reserved in this context"
/// @brief A 64-bit unsigned integer type
// NOLINTNEXTLINE(google-runtime-int,readability-magic-numbers)
using uint64_t = ::std::uint64_t;
// parasoft-end-suppress AUTOSAR-M17_0_2-a-2
// parasoft-end-suppress CERT_CPP-DCL51-f-3
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "False positive: int8_t not reserved in this context"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: int8_t not reserved in this context"
/// @brief An 8-bit signed integer type
using int8_t = ::std::int8_t;
// parasoft-end-suppress AUTOSAR-M17_0_2-a-2
// parasoft-end-suppress CERT_CPP-DCL51-f-3
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "False positive: int16_t not reserved in this context"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: int16_t not reserved in this context"
/// @brief A 16-bit signed integer type
// NOLINTNEXTLINE(google-runtime-int)
using int16_t = ::std::int16_t;
// parasoft-end-suppress AUTOSAR-M17_0_2-a-2
// parasoft-end-suppress CERT_CPP-DCL51-f-3
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "False positive: int32_t not reserved in this context"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: int32_t not reserved in this context"
/// @brief A 32-bit signed integer type
using int32_t = ::std::int32_t;
// parasoft-end-suppress AUTOSAR-M17_0_2-a-2
// parasoft-end-suppress CERT_CPP-DCL51-f-3
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "This code is defining a fixed-width type"
// parasoft-begin-suppress AUTOSAR-M17_0_2-a-2 "False positive: int64_t not reserved in this context"
// parasoft-begin-suppress CERT_CPP-DCL51-f-3 "False positive: int64_t not reserved in this context"
/// @brief A 64-bit signed integer type
// NOLINTNEXTLINE(google-runtime-int,readability-magic-numbers)
using int64_t = ::std::int64_t;
// parasoft-end-suppress AUTOSAR-M17_0_2-a-2
// parasoft-end-suppress CERT_CPP-DCL51-f-3
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPILER_SUPPORT_INTEGER_TYPES_HPP_
