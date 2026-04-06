// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file mutex_test.cpp
/// @brief Validates that expected symbols are exported from mutex.hpp
///

#include "arene/base/mutex.hpp"

#include <gtest/gtest.h>

namespace {

/// @test `mutex` is declared
TEST(Mutex, MutexIsExported) { using arene::base::mutex; }

/// @test `timed_mutex` is declared
TEST(Mutex, TimedMutexIsExported) { using arene::base::timed_mutex; }

/// @test `lock_guard` is declared
TEST(Mutex, LockGuardIsExported) { using arene::base::lock_guard; }

}  // namespace
