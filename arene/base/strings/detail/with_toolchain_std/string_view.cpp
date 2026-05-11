// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/strings/string_view.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

#include <cstddef>
#include <string>

namespace arene {
namespace base {

/// @brief Conversion operator for converting to a @c std::string
/// @return A new @c std::string holding a copy of the string from the view
string_view::operator std::string() const { return {data(), size()}; }

/// @brief Convert a @c string_view to a @c std::string
/// @param str The @c string_view to convert
/// @return A new @c std::string holding a copy of the string from the view
auto to_string(string_view const str) -> std::string { return str; }

/// @brief Special marker value for end of string
constexpr std::size_t string_view::npos;

}  // namespace base
}  // namespace arene
