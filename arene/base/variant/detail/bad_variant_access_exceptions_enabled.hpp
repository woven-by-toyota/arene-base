// parasoft-begin-suppress AUTOSAR-A2_8_1-a "The file contains content related to bad_variant_access."

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_DETAIL_BAD_VARIANT_ACCESS_EXCEPTIONS_ENABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_DETAIL_BAD_VARIANT_ACCESS_EXCEPTIONS_ENABLED_HPP_

// IWYU pragma: private, include "arene/base/variant.hpp"
// IWYU pragma: friend "arene/base/variant/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"
#include "arene/base/variant/traits.hpp"

// parasoft-end-suppress AUTOSAR-A16_2_2-a "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

/// @brief Exception thrown when @c variant::get<T>() is called with an index or type that does not match the current
///        active alternative.
class bad_variant_access : public std::exception {
 public:
  /// @brief Default ctor.
  /// @post what() returns "Unknown reason"
  bad_variant_access() noexcept = default;

  // parasoft-begin-suppress AUTOSAR-A3_9_1-b "This represents a character not a number"
  /// @brief ctor with a message
  /// @param rsn Pointer to the error message char string. Note this *must* be a string with static lifetime.
  /// @post what() returns @c rsn
  explicit bad_variant_access(detail::raw_c_string const rsn) noexcept
      : std::exception{},
        reason_{rsn} {}
  // parasoft-end-suppress AUTOSAR-A3_9_1-b

  /// @brief copy ctor
  /// @param other Const reference to other bad_variant_access
  /// @post what() returns same reason as @c other
  bad_variant_access(bad_variant_access const& other) noexcept = default;

  /// @brief copy assign
  /// @param other Const reference to other bad_variant_access
  /// @post what() returns same reason as @c other
  auto operator=(bad_variant_access const& other) noexcept -> bad_variant_access& = default;

  /// @brief move ctor
  /// @param other Const reference to other bad_variant_access
  /// @post what() returns same reason as @c other
  bad_variant_access(bad_variant_access&& other) noexcept = default;

  /// @brief move assign
  /// @param other Const reference to other bad_variant_access
  /// @post what() returns same reason as @c other
  auto operator=(bad_variant_access&& other) noexcept -> bad_variant_access& = default;

  /// @brief dtor
  ~bad_variant_access() noexcept override = default;

  // parasoft-begin-suppress AUTOSAR-A3_9_1-b "This represents a character not a number"
  /// @brief Get the error message
  /// @return The error message
  auto what() const noexcept -> char const* override { return reason_; }
  // parasoft-end-suppress AUTOSAR-A3_9_1-b

 private:
  /// @brief Pointer to the error message string
  // parasoft-begin-suppress AUTOSAR-A27_0_4-d "'const char *' value constructed from string literal always has the
  // correct buffer size and a static lifetime"
  detail::raw_c_string reason_{"Unknown reason"};
  // parasoft-begin-suppress AUTOSAR-A27_0_4-d "'const char *' value constructed from string literal always has the
  // correct buffer size and a static lifetime"
};

namespace variant_detail {
/// @brief checks if the variant's index is not valueless and holds a value of the given index.
/// @param requested the index of expected object
/// @param index the index of owned object
/// @throws arene::base::bad_variant_access if @c requested!=index or @c index==variant_npos .
inline void variant_must_have_value(std::size_t const requested, std::size_t const index) {
  if (index == variant_npos) {
    throw bad_variant_access{"Valueless by exception"};
  }
  if (index != requested) {
    throw bad_variant_access{"Variant does not hold alternative"};
  }
}
}  // namespace variant_detail

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_VARIANT_DETAIL_BAD_VARIANT_ACCESS_EXCEPTIONS_ENABLED_HPP_
