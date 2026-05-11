// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/contracts/contract.hpp"

namespace {

using arene::base::contract_detail::contract_type;
using arene::base::contract_detail::contract_violation_info;
using arene::base::contract_detail::contract_violation_wrapper;

/// @test When `contract_violation_wrapper` is invoked and stacktrace capture is disabled, stacktrace content is not
/// printed.
TEST(ContractViolationWrapperDeathTest, PrintsStacktraceForAllContractTypes) {
  contract_violation_info info{"test_file", "test_line", "test_message", contract_type::invariant};
  {
    SCOPED_TRACE("Precondition");
    info.type = contract_type::precondition;
    EXPECT_DEATH(contract_violation_wrapper(info), ::testing::Not(::testing::ContainsRegex("google.*googletest")));
  }
  {
    SCOPED_TRACE("Invariant");
    info.type = contract_type::invariant;
    EXPECT_DEATH(contract_violation_wrapper(info), ::testing::Not(::testing::ContainsRegex("google.*googletest")));
  }
  {
    SCOPED_TRACE("Unreachable");
    info.type = contract_type::unreachable;
    EXPECT_DEATH(contract_violation_wrapper(info), ::testing::Not(::testing::ContainsRegex("google.*googletest")));
  }
}

}  // namespace
