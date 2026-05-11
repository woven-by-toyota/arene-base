// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file constraints_test.cpp
/// @brief Provides unit tests to validate export of content in constraints.hpp
///

#include "arene/base/constraints.hpp"

#include <gtest/gtest.h>

namespace {

/// @test Ensure `constraints` is exported
TEST(Constraints, Constraints) { using arene::base::constraints; }

/// @test Ensure `substitution_succeeds` is exported
TEST(Constraints, SubstitutionSucceeds) { using arene::base::substitution_succeeds; }

}  // namespace
