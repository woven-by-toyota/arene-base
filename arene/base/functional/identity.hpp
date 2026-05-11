// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_IDENTITY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_IDENTITY_HPP_

// IWYU pragma: private, include "arene/base/functional.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/stdlib_choice/forward.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: Declaration *is* preceeded by the @brief tag."

/// @brief A backport of the C++20 @c std::identity function object. It returns its input unchanged.
class identity {
 public:
  /// @brief Returns the input value unchanged as if forwarded via std::forward.
  ///
  /// @tparam T The input type.
  /// @param value The input value.
  /// @return T&& The forwarded input value, unchanged.
  template <typename T>
  constexpr auto operator()(T&& value) const noexcept -> T&& {
    return std::forward<T>(value);
  }

  /// @brief Indicates that this function object can be used with transparent comparisons.
  using is_transparent = void;
};

// parasoft-end-suppress AUTOSAR-A2_7_3-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_IDENTITY_HPP_
