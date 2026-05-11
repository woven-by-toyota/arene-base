// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SOURCE_LOCATION_SRC_LINE_INFO_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SOURCE_LOCATION_SRC_LINE_INFO_HPP_

#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/strings/null_terminated_string_view.hpp"
// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// IWYU pragma: private, include "arene/base/source_location.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {

/// @brief Container for the location information associated with a line of source code
struct src_line_info {
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a "False positive: 'src_line_info' is POD"
  /// @brief function name of the parent function
  null_terminated_string_view function_name;
  /// @brief filepath of the source file
  null_terminated_string_view filepath;
  /// @brief line number
  std::uint32_t line{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a
};

}  // namespace base
}  // namespace arene

// AUTOSAR Rule A16-0-1 Exception: File, function and line informations in C
// and C++ are accessible only using the preprocessor. This macro is required
// to extract these informations for the logger purpose

// parasoft-begin-suppress AUTOSAR-A16_0_1-d "Defines function-like macros permitted by A16-0-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A3_9_1-b	"False positive: This represents a character not a number"

/// @def ARENE_GET_SRC_CODE_LOCATION_INFO()
/// Construct @c src_line_info with the current source code information that includes the source code file filepath, the
/// current line, and the function name.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ARENE_GET_SRC_CODE_LOCATION_INFO()                                                         \
  (::arene::base::src_line_info{                                                                   \
      static_cast<::arene::base::null_terminated_string_view>(static_cast<const char*>(__func__)), \
      static_cast<::arene::base::null_terminated_string_view>(static_cast<const char*>(__FILE__)), \
      static_cast<std::uint32_t>(__LINE__)                                                         \
  })

// parasoft-end-suppress AUTOSAR-A3_9_1-b
// parasoft-end-suppress AUTOSAR-A16_0_1-d

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SOURCE_LOCATION_SRC_LINE_INFO_HPP_
