// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONTRACTS_DETAIL_STACKTRACE_NOOP_IMPL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONTRACTS_DETAIL_STACKTRACE_NOOP_IMPL_HPP_

// IWYU pragma: private, include "arene/base/contracts/detail/stacktrace/stacktrace.hpp"
// IWYU pragma: friend "arene/base/contracts/detail/stacktrace/noop/.*"

// parasoft-begin-suppress AUTOSAR-A18_0_1-a-2 "This code needs to work when the C++ Standard Library is not available"
// parasoft-begin-suppress AUTOSAR-A1_1_1-d-2 "This code needs to work when the C++ Standard Library is not available"
// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stdio.h>  // parasoft-suppress AUTOSAR-M27_0_1-a-2 "Using <stdio.h> for output"
// parasoft-end-suppress AUTOSAR-A1_1_1-d-2
// parasoft-end-suppress AUTOSAR-A18_0_1-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

#include "arene/base/stdlib_choice/cstddef.hpp"

namespace arene {
namespace base {
namespace contracts_detail {

/// @brief The native handle type for noop backend is void*
using native_handle_t = void*;

/// @brief noop is a noop.
static constexpr bool stacktrace_is_noop{true};

///
/// @brief Trace implementation for a noop backend. Does nothing.
///
/// @return std::size_t Will always return 0
///
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters) This is the contract for this API.
constexpr auto current_stack_trace_impl(native_handle_t*, std::size_t, std::size_t) noexcept -> std::size_t {
  return {};
}

// parasoft-begin-suppress AUTOSAR-M0_1_8-b "Required for consistency with API of primary template"
///
/// @brief Symbolization implementation for a noop backend. Does nothing.
///
constexpr void symbolize_stack_trace_to_impl(FILE*, native_handle_t const*, std::size_t) noexcept {}
// parasoft-end-suppress AUTOSAR-M0_1_8-b

}  // namespace contracts_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONTRACTS_DETAIL_STACKTRACE_NOOP_IMPL_HPP_
