// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: contains are_exceptions_enabled."

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_ARE_EXCEPTIONS_ENABLED_DISABLED_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_ARE_EXCEPTIONS_ENABLED_DISABLED_HPP_

#include "arene/base/stdlib_choice/integral_constant.hpp"

// IWYU pragma: private, include "arene/base/detail/exceptions/are_exceptions_enabled.hpp"
// IWYU pragma: friend "arene/base/detail/exceptions/.*"

namespace arene {
namespace base {
namespace detail {

/// @brief Trait for testing if @c -fno-exceptions or equivalent build configuration is enabled.
class are_exceptions_enabled : public std::false_type {};

}  // namespace detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_DETAIL_EXCEPTIONS_ARE_EXCEPTIONS_ENABLED_DISABLED_HPP_
