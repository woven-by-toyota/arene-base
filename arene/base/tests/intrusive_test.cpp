// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file intrusive_test.cpp
/// @brief Validates that expected symbols are exported from the "intrusive" subpackage
///

#include <gtest/gtest.h>

#include "arene/base/intrusive/priority_queue.hpp"
#include "arene/base/intrusive/queue.hpp"

namespace {

/// @test Ensure that `priority_queue` is declared
TEST(Intrusive, PriorityQueue) { using ::arene::base::intrusive::priority_queue; }

/// @test Ensure that `queue` is declared
TEST(Intrusive, Queue) { using ::arene::base::intrusive::queue; }

}  // namespace
