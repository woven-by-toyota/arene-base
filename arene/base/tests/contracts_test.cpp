// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file contracts_test.cpp
/// @brief Provides unit tests to validate export of content in contracts.hpp
///

#include "arene/base/contracts.hpp"

#include <gtest/gtest.h>

namespace {

/// @test Ensure `ARENE_PRECONDITION` is exported
TEST(Contracts, ArenePrecondition) {
#ifndef ARENE_PRECONDITION
  GTEST_FAIL() << "ARENE_PRECONDITION is not defined";
#endif
}

/// @test Ensure `ARENE_INVARIANT` is exported
TEST(Contracts, AreneInvariant) {
#ifndef ARENE_INVARIANT
  GTEST_FAIL() << "ARENE_INVARIANT is not defined";
#endif
}

/// @test Ensure `ARENE_INVARIANT_UNREACHABLE` is exported
TEST(Contracts, AreneInvariantUnreachable) {
#ifndef ARENE_INVARIANT_UNREACHABLE
  GTEST_FAIL() << "ARENE_INVARIANT_UNREACHABLE is not defined";
#endif
}

}  // namespace
