// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONTRACTS_DETAIL_STACKTRACE_STACKTRACE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONTRACTS_DETAIL_STACKTRACE_STACKTRACE_HPP_

// parasoft-begin-suppress AUTOSAR-A18_0_1-a-2 "This code needs to work when the C++ Standard Library is not available"
// parasoft-begin-suppress AUTOSAR-A1_1_1-d-2 "This code needs to work when the C++ Standard Library is not available"
// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stdio.h>  // parasoft-suppress AUTOSAR-M27_0_1-a-2 "Using <stdio.h> for output"
// parasoft-end-suppress AUTOSAR-A1_1_1-d-2
// parasoft-end-suppress AUTOSAR-A18_0_1-a-2

#include "arene/base/contracts/detail/stacktrace/impl.hpp"  // IWYU pragma: export
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"

namespace arene {
namespace base {
namespace contracts_detail {

/// @brief The default maximum depth of a stack trace to hold.
static constexpr std::size_t default_max_stack_trace_depth{64U};

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

///
/// @brief A container for holding a stacktrace as a sequence of @c native_handle_t .
///
/// @tparam MaxTraceDepth The maximum depth of the strace to store.
/// @see stacktrace<MaxTraceDepth>::current() for usage instructions.
///
template <std::size_t MaxTraceDepth = default_max_stack_trace_depth>
class stacktrace {
 public:
  // parasoft-begin-suppress AUTOSAR-M11_0_1-a-2 "False positive: this is not 'member data', it is a public property,
  // exposed as part of the API"
  /// @brief The maximum number of elements the stacktrace can hold.
  static constexpr std::integral_constant<std::size_t, MaxTraceDepth> max_size{};
  // parasoft-end-suppress AUTOSAR-M11_0_1-a

  /// @brief The storage used to hold the stacktrace
  using storage_type = native_handle_t[max_size];  // NOLINT(hicpp-avoid-c-arrays) Using array would create circular dep

  /// @brief The value type of the data in the stack trace
  using value_type = native_handle_t;
  /// @brief The reference type of data in the stack trace
  using reference = native_handle_t&;
  /// @brief The const reference type of data in the stack trace
  using const_reference = native_handle_t const&;

  /// @brief The type of the const iterator over the stack trace
  using const_iterator = native_handle_t const*;
  /// @brief The type of the iterator over the stack trace
  using iterator = const_iterator;

  /// @brief The type returned by @c size()
  using size_type = std::size_t;

  /// @brief The type returned by differencing two pointers
  using difference_type = std::ptrdiff_t;

  /// @brief Query if the stacktrace has content.
  /// @return true If @c size()==0U
  /// @return false If @c size()!=0U
  auto empty() const -> bool { return size() == 0U; }

  /// @brief Query the number of elements in the stacktrace
  /// @return size_type The number of elements in the stacktrace. Will always be @c <=max_size .
  auto size() const -> size_type { return size_; }

  /// @brief Access the raw data in the stacktrace
  /// @return value_type const* A const pointer to the first element in the stacktrace.
  auto data() const -> value_type const* { return frames_; }

  /// @brief Access an iterator to the first entry in the stacktrace.
  /// @return const_iterator An iterator pointing to the first element in the stacktrace.
  auto begin() const -> const_iterator { return data(); }

  // parasoft-begin-suppress AUTOSAR-M5_0_15-a "The incremented pointer does point to an array"
  /// @brief Access an iterator to one-past the final entry in the stacktrace.
  /// @return const_iterator An iterator pointing one-past the final element in the stacktrace.
  auto end() const -> const_iterator { return begin() + static_cast<std::ptrdiff_t>(size()); }
  // parasoft-end-suppress AUTOSAR-M5_0_15-a

  ///
  /// @brief Capture a stacktrace from the current source location.
  ///
  /// @param skip_first_n_frames The number of frames in the stacktrace to skip. This can be useful for example when
  ///        making a helper function for capturing stacktraces where you do not want to include the function itself in
  ///        the trace.
  /// @return stacktrace A stacktrace instance which will contain information available on the stacktrace to the
  ///         location of the call to @c current() . If the stack trace exceeds @c max_size it will be truncated.
  /// @note Capturing stacktraces is platform and optimization level dependent. The only promises this interface provide
  ///       is that the trace will contain between @c 0 and @c max_size trace elements; that is, that the returned trace
  ///       will be subset of possible valid traces.
  ///
  static auto current(std::size_t skip_first_n_frames = 0U) noexcept -> stacktrace {
    constexpr std::size_t skip_frames_for_self{1U};
    stacktrace trace;
    trace.size_ = current_stack_trace_impl(
        trace.frames_,
        trace.max_size(),
        std::max(skip_first_n_frames, skip_first_n_frames + skip_frames_for_self)
    );
    return trace;
  }

 private:
  /// @brief An array of handles which represent stack frames. Index 0 is the "top" of the stack.
  // parasoft-begin-suppress AUTOSAR-A18_1_1-a "This must work in contexts where std::array is not available."
  storage_type frames_{};
  // parasoft-end-suppress AUTOSAR-A18_1_1-a "This must work in contexts where std::array is not available."
  /// @brief The actual number of stack frames stored.
  size_type size_{0U};
};

template <std::size_t MaxTraceDepth>
constexpr std::integral_constant<std::size_t, MaxTraceDepth> stacktrace<MaxTraceDepth>::max_size;

///
/// @brief Symbolizes a stacktrace to an output file
///
/// @tparam MaxTraceDepth The maximum depth of the trace.
/// @param file The file pointer to write to. If @c nullptr this is a noop.
/// @param trace The trace to symbolize.
/// @pre If @c file is not @c nullptr , it must be a file opened for writing to the location the trace should be written
///      to.
/// @post @c file will contain a platform-dependent symbolized representation of the stack trace.
/// @note Symbolizing a stacktrace is highly platform dependent, and may not be possible.
template <std::size_t MaxTraceDepth>
void symbolize_stack_trace_to(FILE* file, stacktrace<MaxTraceDepth> const& trace) noexcept {
  if (file == nullptr) {
    return;
  }
  symbolize_stack_trace_to_impl(file, trace.data(), trace.size());
}

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

}  // namespace contracts_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_CONTRACTS_DETAIL_STACKTRACE_STACKTRACE_HPP_
