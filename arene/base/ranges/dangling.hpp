// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DANGLING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DANGLING_HPP_

// IWYU pragma: private, include "arene/base/ranges.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

namespace arene {
namespace base {

namespace ranges {

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

/// @brief A type that can be used to indicate that a range is dangling, i.e. that it is not safe to use the range's
/// iterators or references after the range has been destroyed.
class dangling {
 public:
  /// @brief Default constructor
  constexpr dangling() noexcept = default;

  // parasoft-begin-suppress AUTOSAR-A13_3_1-a "False Positive: Constrained via SFINAE"
  /// @brief Implicitly constructible from any argument list
  /// @tparam Args the types of the arguments to swallow
  template <typename... Args>
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions) dangling must be implicitly constructible
  constexpr dangling(Args&&...) noexcept {}

  /// @brief Copy constructor
  constexpr dangling(dangling const&) noexcept = default;

  /// @brief Move constructor
  constexpr dangling(dangling&&) noexcept = default;

  /// @brief Copy assignment operator
  constexpr auto operator=(dangling const&) noexcept -> dangling& = default;

  /// @brief Move assignment operator
  constexpr auto operator=(dangling&&) noexcept -> dangling& = default;
  // parasoft-end-suppress AUTOSAR-A13_3_1-a

  /// @brief Destructor
  ~dangling() noexcept = default;
};
// parasoft-end-suppress AUTOSAR-A7_1_5-a-2

}  // namespace ranges
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_RANGES_DANGLING_HPP_
