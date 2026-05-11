// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_WITH_TOOLCHAIN_STD_INLINE_STRING_STD_STRING_MIXIN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_WITH_TOOLCHAIN_STD_INLINE_STRING_STD_STRING_MIXIN_HPP_

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// IWYU pragma: private
// IWYU pragma: friend "arene/base/strings/inline_string.hpp"

#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/stdlib_choice/string.hpp"
#include "arene/base/strings/string_view.hpp"

namespace arene {
namespace base {

/// @brief Helper class to provde std::string comparisons for inline strings.
/// @tparam StringView The string view type to provide the comparisons for.
template <class InlineString>
class inline_string_std_string_mixin {
 protected:
  /// @brief Compare two strings for lexicographical ordering.
  ///
  /// @param lhs The first string
  /// @param rhs The second string
  /// @return strong_ordering::equal If both strings have the same @c size() and equivalent characters.
  /// @return strong_ordering::less If @c lhs is lexicographically before @c rhs .
  /// @return strong_ordering::greater If @c lhs is lexicographically after @c rhs .
  ARENE_NODISCARD static constexpr auto three_way_compare(InlineString const& lhs, std::string const& rhs) noexcept
      -> strong_ordering {
    return string_view::three_way_compare(lhs, string_view{rhs});
  }

  /// @brief Determine if two strings are definitely not equal.
  ///
  /// @param lhs The first string
  /// @param rhs The second string
  /// @return inequality_heuristic::definitely_not_equal If @c lhs.size()!=rhs.size() .
  /// @return inequality_heuristic::may_be_equal_or_not_equal If @c lhs.size()==rhs.size() .
  ARENE_NODISCARD static constexpr auto fast_inequality_check(InlineString const& lhs, std::string const& rhs) noexcept
      -> inequality_heuristic {
    return string_view::fast_inequality_check(lhs, string_view{rhs});
  }
};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_WITH_TOOLCHAIN_STD_INLINE_STRING_STD_STRING_MIXIN_HPP_
