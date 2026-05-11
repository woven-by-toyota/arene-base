// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::ignore"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IGNORE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IGNORE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <tuple>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/cstddef.hpp"

namespace std {
namespace ignore_detail {
/// @brief A class that anything can be assigned to, and which discards the assigned object
class ignore_t {
 public:
  /// @brief Default constructor
  constexpr ignore_t() = default;
  /// @brief Default destructor
  ~ignore_t() = default;

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Default copy-constructor
  constexpr ignore_t(ignore_t const&) = default;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Default move-constructor
  constexpr ignore_t(ignore_t&&) = default;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  // parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
  /// @brief Default copy-assignment
  constexpr auto operator=(ignore_t const&) noexcept -> ignore_t& = default;
  // parasoft-end-suppress AUTOSAR-A7_1_5-a-2
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  // parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
  /// @brief Default move-constructor
  constexpr auto operator=(ignore_t&&) noexcept -> ignore_t& = default;
  // parasoft-end-suppress AUTOSAR-A7_1_5-a-2
  // parasoft-end-suppress CERT_C-EXP37-a-3

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Do-nothing catch-all assignment operator
  /// @tparam Arg The type of the argument to ignore
  /// @return A reference to @c *this
  template <typename Arg>
  // NOLINTNEXTLINE(misc-unconventional-assign-operator)
  constexpr auto operator=(Arg const&) const noexcept -> ignore_t const& {
    return *this;
  }
  // parasoft-end-suppress CERT_C-EXP37-a-3
};

/// @brief An object to which anything can be assigned, which ignores the assigned value
template <nullptr_t = nullptr>
extern constexpr ignore_t ignore{};

}  // namespace ignore_detail

// parasoft-begin-suppress AUTOSAR-M3_4_1-b-2 "This is a per-TU reference to a global used in multiple TUs"
// parasoft-begin-suppress CERT_CPP-DCL56-a-3 "False positive: variable is initialized"
/// @brief An object to which anything can be assigned, which ignores the assigned value
static constexpr auto const& ignore = ignore_detail::ignore<>;
// parasoft-end-suppress CERT_CPP-DCL56-a-3
// parasoft-end-suppress AUTOSAR-M3_4_1-b-2

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IGNORE_HPP_
