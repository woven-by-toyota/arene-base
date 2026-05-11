// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mutex/timed_mutex.hpp"

#include <mutex>

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

namespace {

using ::arene::base::timed_mutex;

/// @test `timed_mutex` is the same as `std::timed_mutex` if TSA is disabled
TEST(LockGuard, IsStdLockGuardIfTsaIsNotSupported) {
#if ARENE_IS_OFF(ARENE_HAS_THREAD_SAFETY_ANALYSIS)
  ::testing::StaticAssertTypeEq<timed_mutex, std::timed_mutex>();
#else
  GTEST_SKIP() << "Only meaningful when TSA is disabled.";
#endif
}

/// @test `timed_mutex` is the same as `std::timed_mutex` if using libcxx
TEST(LockGuard, IsStdLockGuardIfOnLibcxx) {
#if ARENE_IS_ON(ARENE_STDLIB_LIBCXX)
  ::testing::StaticAssertTypeEq<timed_mutex, std::timed_mutex>();
#else
  GTEST_SKIP() << "Only meaningful when on libcxx.";
#endif
}

/// @test `timed_mutex` is the same as an a detail type if TSA is enabled and not using libcxx
TEST(LockGuard, IsWrapperIfTsaIsSupportedAndNotOnLibcxx) {
#if ARENE_IS_ON(ARENE_HAS_THREAD_SAFETY_ANALYSIS) && ARENE_IS_OFF(ARENE_STDLIB_LIBCXX)
  ::testing::StaticAssertTypeEq<timed_mutex, ::arene::base::mutex_detail::timed_mutex<std::timed_mutex>>();
#else
  GTEST_SKIP() << "Only meaningful when TSA is enabled an we're not on libcxx.";
#endif
}

}  // namespace
