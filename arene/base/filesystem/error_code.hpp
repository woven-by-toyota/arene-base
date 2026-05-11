// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_ERROR_CODE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_ERROR_CODE_HPP_

#include <cstddef>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare.hpp"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/inline_string.hpp"
#include "arene/base/string_view.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

namespace filesystem {

// parasoft-begin-suppress AUTOSAR-A5_1_1-a-2 "False positive: literal is used to initialize a constant"
/// @brief The maximum length of an error message
constexpr std::size_t max_error_length{128U};
// parasoft-end-suppress AUTOSAR-A5_1_1-a-2

/// @brief An inline_string large enough to hold an error message
using error_string = inline_string<max_error_length>;

/// @brief A class representing a filesystem error value. Intended to be a wrapper around errno.
class error_code {
 public:
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "Using an int as that is the type of errno"
  /// @brief Raw error code type
  using raw_error_code = int;
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

  /// @brief Construct with no error
  /// @post <c> value() == 0 </c>
  error_code() noexcept
      : error_code(0) {}

  // parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
  /// @brief Construct with a specified error value
  /// @param error The error value to construct from
  /// @post <c> value() == error </c>
  constexpr explicit error_code(raw_error_code const error) noexcept
      : error_(error) {}
  // parasoft-end-suppress AUTOSAR-M2_10_1-a-2

  /// @brief Create an instance from the current state of @c errno.
  /// @return An @c error_code with @c value() equal to the old value of @c errno
  /// @post Sets @c errno to 0
  static auto from_errno() noexcept -> error_code;

  /// @brief Get the stored error value
  /// @return The stored error value
  constexpr auto value() const noexcept -> raw_error_code { return error_; }

  /// @brief Check if the error is non-zero, which represents there _is_ an error.
  /// @return true if the error value is non-zero
  /// @return false otherwise.
  explicit operator bool() const noexcept { return value() != 0; }

  /// @brief clear the error
  /// @post <c> value() == 0 </c>
  /// @post <c> static_cast<bool>(*this) == false </c>
  void clear() noexcept { error_ = 0; }

  /// @brief Get a message describing the error
  /// @return An @c error_string with a description of the error indicated by the stored error value
  auto message() const noexcept -> error_string;

  /// @brief Raise an exception produced from the error.
  /// @throws std::system_error containing the error code
  // clang-format off
  ARENE_NORETURN void throw_error() const; // parasoft-suppress AUTOSAR-A15_4_5-a-2 "suppress AUTOSAR-A15_4_5-a-2 due to confusing std::__1::system_error and std::system_error"
  // clang-format on

  /// @brief Raise an exception produced from the error with a descriptive message.
  /// @param message_prefix A string to prefix the error message with
  /// @throws std::system_error containing the error code prefixed with the given message.
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  // clang-format off
  // NOLINTNEXTLINE(readability-avoid-const-params-in-decls)
  ARENE_NORETURN void throw_error_with_prefix(null_terminated_string_view const message_prefix) const; // parasoft-suppress AUTOSAR-A15_4_5-a-2 "Confusing std::__1::system_error and std::system_error"
  // clang-format on

 private:
  /// @brief The stored error value
  raw_error_code error_;
};

}  // namespace filesystem
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FILESYSTEM_ERROR_CODE_HPP_
