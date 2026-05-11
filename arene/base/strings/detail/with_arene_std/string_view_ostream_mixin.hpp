// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: Defines class string_view_ostream_mixin"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_WITH_ARENE_STD_STRING_VIEW_OSTREAM_MIXIN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_WITH_ARENE_STD_STRING_VIEW_OSTREAM_MIXIN_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/strings/string_view.hpp"

namespace arene {
namespace base {

/// @brief Helper class to provde an ostream operator for string views.
/// @tparam StringView The string view type to provide the operator for.
template <class StringView>
class string_view_ostream_mixin {};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_DETAIL_WITH_ARENE_STD_STRING_VIEW_OSTREAM_MIXIN_HPP_
