// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/contracts.hpp"
#include "testlibs/minitest/minitest.hpp"

namespace {

/// @test check that arene contract macros are usable when *not* using the
/// toolchain provided C++ standard library.
TEST(AreneBaseContracts, UsableWithoutToolchainStdlib) { ARENE_PRECONDITION(true); }

}  // namespace
