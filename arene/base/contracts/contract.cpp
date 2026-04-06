// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/contracts/contract.hpp"

// parasoft-begin-suppress AUTOSAR-A18_0_1-a-2 "This code needs to work when the C++ Standard Library is not available"
// parasoft-begin-suppress AUTOSAR-A1_1_1-d-2 "This code needs to work when the C++ Standard Library is not available"
// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stdio.h>  // parasoft-suppress AUTOSAR-M27_0_1-a-2 "Using <stdio.h> for output"
// parasoft-end-suppress AUTOSAR-A1_1_1-d-2
// parasoft-end-suppress AUTOSAR-A18_0_1-a-2

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/terminate.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A3_1_2-a-2 "Including implementation file rather than public header"
#if ARENE_IS_ON(ARENE_IS_GCOV)
#include "arene/base/contracts/detail/pre_abort_cleanup_gcov.inl"
#elif ARENE_IS_ON(ARENE_IS_LLVMCOV)
#include "arene/base/contracts/detail/pre_abort_cleanup_llvmcov.inl"
#else
#include "arene/base/contracts/detail/pre_abort_cleanup_production.inl"
#endif
// parasoft-end-suppress AUTOSAR-A3_1_2-a-2

#include "arene/base/contracts/detail/stacktrace/stacktrace.hpp"

namespace arene {
namespace base {
namespace contract_detail {

// parasoft-begin-suppress AUTOSAR-A27_0_4-d-2 "Passing string literals to fputs"
/// @brief This function is called when a contract is violated. It reports the violation and terminates the program.
/// @param info Information about the contract violation
ARENE_NORETURN void contract_violation(contract_violation_info const& info) noexcept {
  std::ignore = fputs(info.file, stderr);
  std::ignore = fputc(':', stderr);
  std::ignore = fputs(info.line, stderr);
  std::ignore = fputs(": error: ", stderr);
  switch (info.type) {
    case contract_type::precondition:
      std::ignore = fputs("Precondition violation: ", stderr);
      break;
    case contract_type::invariant:
      std::ignore = fputs("Invariant violation: ", stderr);
      break;
    case contract_type::unreachable:
      std::ignore = fputs("Unreachable code reached: ", stderr);
      break;
  }
  std::ignore = fputs(info.message, stderr);
  std::ignore = fputc('\n', stderr);
  contracts_detail::symbolize_stack_trace_to(stderr, contracts_detail::stacktrace<>::current(1U));
  std::ignore = fflush(stderr);
  contracts_detail::pre_abort_cleanup();
  std::terminate();
}
// parasoft-end-suppress AUTOSAR-A27_0_4-d-2

}  // namespace contract_detail
}  // namespace base
}  // namespace arene
