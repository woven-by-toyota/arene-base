// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: Defines class string_view_ostream_mixin"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_WITH_TOOLCHAIN_STD_STRING_VIEW_OSTREAM_MIXIN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_WITH_TOOLCHAIN_STD_STRING_VIEW_OSTREAM_MIXIN_HPP_

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// IWYU pragma: private
// IWYU pragma: friend "arene/base/strings/string_view.hpp"

#include <ios>
#include <ostream>

#include "arene/base/stdlib_choice/ignore.hpp"

namespace arene {
namespace base {

/// @brief Helper class to provde an ostream operator for string views.
/// @tparam StringView The string view type to provide the operator for.
template <class StringView>
class string_view_ostream_mixin {
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "Hidden friends permitted by A11-3-1 Permit #2"
  // parasoft-begin-suppress AUTOSAR-A7_1_1-a "False positive: lhs is written to"
  // parasoft-begin-suppress AUTOSAR-M7_1_2-c "False positive: lhs is written to"
  // parasoft-begin-suppress AUTOSAR-A8_4_9-a "False positive: lhs is written to"
  // parasoft-begin-suppress AUTOSAR-A0_1_3-a "False positive: Function is namespace scope and used in other translation
  // units."
  // parasoft-begin-suppress AUTOSAR-A7_1_3-a "False positive: const is placed on the right hand side"
  /// @brief IOstream output operator for @c string_view
  /// @param output The stream to write to
  /// @param str The string view to write
  /// @return @c output
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  friend auto operator<<(std::ostream& output, StringView const str) -> std::ostream& {
    std::ignore = output.write(str.data(), static_cast<std::streamsize>(str.size()));
    return output;
  }
  // parasoft-end-suppress AUTOSAR-A7_1_3-a
  // parasoft-end-suppress AUTOSAR-A0_1_3-a
  // parasoft-end-suppress AUTOSAR-A8_4_9-a
  // parasoft-end-suppress AUTOSAR-M7_1_2-c
  // parasoft-end-suppress AUTOSAR-A7_1_1-a
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2
};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_WITH_TOOLCHAIN_STD_STRING_VIEW_OSTREAM_MIXIN_HPP_
