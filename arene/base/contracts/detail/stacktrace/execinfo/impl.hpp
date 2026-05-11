// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONTRACTS_DETAIL_STACKTRACE_EXECINFO_IMPL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONTRACTS_DETAIL_STACKTRACE_EXECINFO_IMPL_HPP_

// IWYU pragma: private, include "arene/base/contracts/detail/stacktrace/stacktrace.hpp"
// IWYU pragma: friend "arene/base/contracts/detail/stacktrace/execinfo/.*"

// parasoft-begin-suppress AUTOSAR-A18_0_1-a-2 "This code needs to work when the C++ Standard Library is not available"
// parasoft-begin-suppress AUTOSAR-A1_1_1-d-2 "This code needs to work when the C++ Standard Library is not available"
// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stdio.h>  // parasoft-suppress AUTOSAR-M27_0_1-a-2 "Using <stdio.h> for output"
// parasoft-end-suppress AUTOSAR-A1_1_1-d-2
// parasoft-end-suppress AUTOSAR-A18_0_1-a-2

#include "arene/base/stdlib_choice/cstddef.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {
namespace contracts_detail {

/// @brief The native handle type for backtrace() is void*
using native_handle_t = void*;

/// @brief backtrace() is not a noop.
static constexpr bool stacktrace_is_noop{false};

///
/// @brief Implementation of a stack trace backend using execinfo's backtrace()
///
/// @param frames Pointer to a buffer of void*'s to write the stack trace to. If @c nullptr , this is a noop.
/// @param max_trace_depth The size of the buffer.
/// @param skip_first_n_frames The number of frames from the trace which should be skipped. This number should not
///                            include skipping this function.
/// @return std::size_t The actual number of frames written to the @c frames buffer, which will always be
//          in the range @c [0,max_trace_depth]. If @c frames==nullptr , then 0.
///
// NOLINTBEGIN(bugprone-easily-swappable-parameters) This is the contract for this API.
// NOLINTBEGIN(readability-avoid-const-params-in-decls) AUTOSAR-M3_9_1-a requires definition and declaration to match.
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax pmeritted by A7-1-5 Permit #1 v1.0.0"
auto current_stack_trace_impl(
    native_handle_t* const frames,
    std::size_t const max_trace_depth,
    std::size_t const skip_first_n_frames
) noexcept -> std::size_t;
// parasoft-end-suppress AUTOSAR-A7_1_5-a
// NOLINTEND(readability-avoid-const-params-in-decls)
// NOLINTEND(bugprone-easily-swappable-parameters)

///
/// @brief Implementation of symbolizing a stack trace using execinfo's backtrace_symbols_fd()
///
/// @param file Pointer to the file object to write the symbolized trace to.
/// @param frames Pointer to a buffer of @c void* 's which represent a stack trace captured by @c backtrace()
/// @param trace_depth The size of the buffer of frames to symbolize.
/// @pre @c frames must point to valid memory of at least @c trace_depth elements, else behavior is undefined.
/// @post backtrace_symbols_fd() is invoked directly against @c file .
///
// NOLINTBEGIN(readability-avoid-const-params-in-decls) AUTOSAR-M3_9_1-a requires definition and declaration to match.
void symbolize_stack_trace_to_impl(
    FILE* const file,
    native_handle_t const* const frames,
    std::size_t const trace_depth
) noexcept;
// NOLINTEND(readability-avoid-const-params-in-decls)

}  // namespace contracts_detail
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONTRACTS_DETAIL_STACKTRACE_EXECINFO_IMPL_HPP_
