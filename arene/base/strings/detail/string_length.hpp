// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::string_length"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_STRING_LENGTH_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_STRING_LENGTH_HPP_

// IWYU pragma: private, include "arene/base/string_algorithms.hpp"
// IWYU pragma: friend "arene/base/strings/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace detail {

// parasoft-begin-suppress AUTOSAR-A2_10_1-a-2 "False positive: No global or namespace entity named 'value'"
/// @brief Determine the length of a NUL-terminated string in a constexpr-friendly fashion.
/// @param value the string to take the length of
/// @return the number of characters before the first NUL
/// @pre value is not <c>nullptr</c>, and points to a NUL-terminated string
inline constexpr auto string_length(raw_c_string value) noexcept -> std::size_t {
  ARENE_PRECONDITION(value != nullptr);
  std::size_t length{0U};
  while (*value != '\0') {
    value = ::arene::base::next(value);
    ++length;
  }
  return length;
}
// parasoft-end-suppress AUTOSAR-A2_10_1-a-2

}  // namespace detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_STRING_LENGTH_HPP_
