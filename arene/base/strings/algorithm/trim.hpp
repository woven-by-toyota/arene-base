// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_ALGORITHM_TRIM_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_ALGORITHM_TRIM_HPP_

// IWYU pragma: private, include "arene/base/string_algorithms.hpp"
// IWYU pragma: friend "arene/base/strings/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/strings/inline_string.hpp"
#include "arene/base/strings/string_view.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

/// @brief Characters defined as whitespace to be trimmed
/// @return sequence of whitespace characters
inline constexpr auto whitespace_chars() -> string_view { return {" \f\n\r\t\v"}; }

/// @brief Create a copy of the string view with leading whitespace removed
/// @param str The string to be trimmed
/// @return A copy of the input string with leading whitespace removed. Whitespace is defined by
///          @c arene::base::whitespace_chars . An empty or all whitespace string will result in an empty return string.
constexpr auto ltrim(string_view const str) -> string_view {
  std::size_t const start{str.find_first_not_of(whitespace_chars())};
  if (start == string_view::npos) {
    return string_view{};
  }
  return str.substr(start);
}

/// @brief Create a copy of the string view with leading whitespace removed
/// @tparam MaxLength The size of the inline string
/// @param str The string to be trimmed
/// @return A copy of the input string with leading whitespace removed. Whitespace is defined by
///          @c arene::base::whitespace_chars . An empty or all whitespace string will result in an empty return string.
template <std::size_t MaxLength>
constexpr auto ltrim(inline_string<MaxLength> const& str) -> inline_string<MaxLength> {
  return inline_string<MaxLength>{ltrim(string_view{str})};
}
///

/// @brief Create a copy of the string view with trailing whitespace removed
/// @param str The string_view reference to be trimmed
/// @return A copy of the input string with trailing whitespace removed. Whitespace is defined by
///          @c arene::base::whitespace_chars . An empty or all whitespace string will result in an empty return string.
constexpr auto rtrim(string_view const str) -> string_view {
  std::size_t const end{str.find_last_not_of(whitespace_chars())};
  if (end == string_view::npos) {
    return string_view{};
  }
  return str.substr(0U, end + 1U);
}

/// @brief Create a copy of the string view with trailing whitespace removed
/// @tparam MaxLength The size of the inline string
/// @param str The string_view reference to be trimmed
/// @return A copy of the input string with trailing whitespace removed. Whitespace is defined by
///          @c arene::base::whitespace_chars . An empty or all whitespace string will result in an empty return string.
template <std::size_t MaxLength>
constexpr auto rtrim(inline_string<MaxLength> const& str) -> inline_string<MaxLength> {
  return inline_string<MaxLength>{rtrim(string_view{str})};
}

/// @brief Create a copy of the string view with leading and trailing whitespace removed
/// @param str The string_view reference to be trimmed
/// @return A copy of the input string with leading and trailing whitespace removed. Whitespace is defined by
///          @c arene::base::whitespace_chars . An empty or all whitespace string will result in an empty return string.
constexpr auto trim(string_view const str) -> string_view { return ltrim(rtrim(str)); }

/// @brief Create a copy of the string view with leading and trailing whitespace removed
/// @tparam MaxLength The size of the inline string
/// @param str The string_view reference to be trimmed
/// @return A copy of the input string with leading and trailing whitespace removed. Whitespace is defined by
///          @c arene::base::whitespace_chars . An empty or all whitespace string will result in an empty return string.
template <std::size_t MaxLength>
constexpr auto trim(inline_string<MaxLength> const& str) -> inline_string<MaxLength> {
  return inline_string<MaxLength>{trim(string_view{str})};
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_ALGORITHM_TRIM_HPP_
