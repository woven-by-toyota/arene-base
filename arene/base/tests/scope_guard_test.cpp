// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/scope_guard.hpp"

#include <gtest/gtest.h>

namespace {

/// @test Ensure `scope_guard` is exported
TEST(ScopeGuard, ScopeGuardIsExported) { using ::arene::base::scope_guard; }

/// @test Ensure `on_scope_exit` is exported
TEST(ScopeGuard, OnScopeExitIsExported) { using ::arene::base::on_scope_exit; }

}  // namespace
