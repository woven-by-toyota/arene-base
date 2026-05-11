// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/terminate.hpp"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
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

namespace arene {
namespace base {
namespace contract_detail {

/// @brief This function is called when a contract is violated. It reports the violation and terminates the program.
/// @param info Information about the contract violation
ARENE_NORETURN void contract_violation(contract_violation_info const& info) noexcept {
  std::ignore = info;
  contracts_detail::pre_abort_cleanup();
  std::terminate();
}

}  // namespace contract_detail
}  // namespace base
}  // namespace arene
