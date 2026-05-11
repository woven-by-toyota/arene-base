// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_WITH_ARENE_STD_INLINE_STRING_STD_STRING_MIXIN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_WITH_ARENE_STD_INLINE_STRING_STD_STRING_MIXIN_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/strings/inline_string.hpp"

#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/attributes.hpp"

namespace arene {
namespace base {

/// @brief Helper class to provde std::string comparisons for inline strings.
/// @tparam StringView The string view type to provide the comparisons for.
template <class InlineString>
class inline_string_std_string_mixin {
 private:
  struct dummy {};

 protected:
  /// @brief Dummy definition of @c three_way_compare for cases when @c std::string is not available
  /// @return strong_ordering::equal (but it's not actually possible to call this)
  ARENE_NODISCARD static constexpr auto three_way_compare(dummy, dummy) noexcept -> strong_ordering {
    return strong_ordering::equal;
  }

  /// @brief Dummy definition of @c fast_inequality_check for cases when @c std::string is not available
  /// @return inequality_heuristic::may_be_equal_or_not_equal (but it's not actually possible to call this)
  ARENE_NODISCARD static constexpr auto fast_inequality_check(dummy, dummy) noexcept -> inequality_heuristic {
    return inequality_heuristic::may_be_equal_or_not_equal;
  }
};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_WITH_ARENE_STD_INLINE_STRING_STD_STRING_MIXIN_HPP_
