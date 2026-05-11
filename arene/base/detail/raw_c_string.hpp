// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_RAW_C_STRING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_RAW_C_STRING_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/.*"

namespace arene {
namespace base {
namespace detail {
/// @brief A type alias for a character in a string
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented"
using character = char;  // parasoft-suppress AUTOSAR-A3_9_1-b-2 "This represents a character not a number"
// parasoft-end-suppress AUTOSAR-A2_7_3
/// @brief A type alias for a raw NUL-terminated C string
// parasoft-begin-suppress AUTOSAR-A2_7_3 "False positive: documented above with brief"
using raw_c_string = character const*;
// parasoft-end-suppress AUTOSAR-A2_7_3

}  // namespace detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_RAW_C_STRING_HPP_
