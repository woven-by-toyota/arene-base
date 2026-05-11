// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file monostate_test.cpp
/// @brief Validates that expected symbols are exported from monostate.hpp
///

#include "arene/base/monostate.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `monostate` is declared
TEST(Monostate, Monostate) { using arene::base::monostate; }

/// @test `monostate_identity` is declared
TEST(Monostate, MonostateIdentity) { using arene::base::monostate_identity; }

}  // namespace
