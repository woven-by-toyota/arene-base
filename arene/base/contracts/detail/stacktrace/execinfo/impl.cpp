// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/contracts/detail/stacktrace/execinfo/impl.hpp"

// parasoft-begin-suppress AUTOSAR-A18_0_1-a-2 "This code needs to work when the C++ Standard Library is not available"
// parasoft-begin-suppress AUTOSAR-A1_1_1-d-2 "This code needs to work when the C++ Standard Library is not available"
// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stdio.h>  // parasoft-suppress AUTOSAR-M27_0_1-a-2 "Using <stdio.h> for output"

// parasoft-end-suppress AUTOSAR-A1_1_1-d-2
// parasoft-end-suppress AUTOSAR-A18_0_1-a-2

#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/max_value_overload.hpp"
#include "arene/base/stdlib_choice/min_value_overload.hpp"
#include "arene/base/stdlib_choice/move_algorithm.hpp"

extern "C" {
#include <execinfo.h>
}

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {
namespace contracts_detail {

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "Everything is documented in the header file"
// parasoft-begin-suppress AUTOSAR-A2_7_3-b-2 "Everything is documented in the header file"
// parasoft-begin-suppress AUTOSAR-A15_4_5-a-2 "Everything is documented in the header file"

// NOLINTBEGIN(bugprone-easily-swappable-parameters) This is the contract for this API.
auto current_stack_trace_impl(
    native_handle_t* const frames,
    std::size_t const max_trace_depth,
    std::size_t const skip_first_n_frames
) noexcept -> std::size_t
// NOLINTEND(bugprone-easily-swappable-parameters)
{
  static_assert(
      std::is_same<native_handle_t, void*>::value,
      "For backtrace() based implementation, native handle must be void*"
  );
  if (frames == nullptr) {
    return 0U;
  }
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b "int is the type required by the underlying C interface"
  auto const raw_frames_captured = backtrace(frames, static_cast<int>(max_trace_depth));
  // parasoft-end-suppress AUTOSAR-A3_9_1-b
  auto const frames_captured = static_cast<std::size_t>(std::max(raw_frames_captured, 0));
  auto const num_frames_to_skip = std::min(skip_first_n_frames, frames_captured);

  // parasoft-begin-suppress CERT_C-ARR39-b "This is used in the implementation of next()"
  // parasoft-begin-suppress AUTOSAR-M5_0_15-a "This is used in the implementation of next()"
  // parasoft-begin-suppress AUTOSAR-A18_9_3-a "False positive: this is the algorithm move."
  // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) this is used in the implementation of next()
  auto* const end_frame = std::move(
      frames + static_cast<std::ptrdiff_t>(num_frames_to_skip),
      frames + static_cast<std::ptrdiff_t>(frames_captured),
      frames
  );
  // parasoft-end-suppress AUTOSAR-A18_9_3-a "False positive: this is the algorithm move."

  return static_cast<std::size_t>(std::max(end_frame - frames, 0L));
  // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  // parasoft-end-suppress CERT_C-ARR39-b "this is used in the implementation of next()"
  // parasoft-end-suppress AUTOSAR-M5_0_15-a "this is used in the implementation of next()"
}

void symbolize_stack_trace_to_impl(
    FILE* const file,
    native_handle_t const* const frames,
    std::size_t const trace_depth
) noexcept {
  if ((file != nullptr) && (frames != nullptr)) {
    // parasoft-begin-suppress AUTOSAR-A3_9_1-b "int is the type required by the underlying C interface"
    backtrace_symbols_fd(frames, static_cast<int>(trace_depth), fileno(file));
    // parasoft-end-suppress AUTOSAR-A3_9_1-b
  }
}

// parasoft-end-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-end-suppress AUTOSAR-A2_7_3-a-2 "Everything is documented in the header file"
// parasoft-end-suppress AUTOSAR-A2_7_3-b-2 "Everything is documented in the header file"
// parasoft-end-suppress AUTOSAR-A15_4_5-a-2 "Everything is documented in the header file"

}  // namespace contracts_detail
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
