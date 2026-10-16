// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mutex/lock_guard.hpp"

#include <mutex>

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

namespace {

using ::arene::base::lock_guard;

/// @test Without thread safety analysis support, `arene::base::lock_guard<T>` is `std::lock_guard<T>`
TEST(LockGuard, IsStdLockGuardIfTsaIsNotSupported) {
#if ARENE_IS_OFF(ARENE_HAS_THREAD_SAFETY_ANALYSIS)
  ::testing::StaticAssertTypeEq<lock_guard<std::mutex>, std::lock_guard<std::mutex> >();
#else
  GTEST_SKIP() << "Only meaningful when TSA is disabled.";
#endif
}

/// @test On libc++, `arene::base::lock_guard<T>` is `std::lock_guard<T>`, whose annotations libc++ already
/// provides
TEST(LockGuard, IsStdLockGuardIfOnLibcxx) {
#if ARENE_IS_ON(ARENE_STDLIB_LIBCXX)
  ::testing::StaticAssertTypeEq<lock_guard<std::mutex>, std::lock_guard<std::mutex> >();
#else
  GTEST_SKIP() << "Only meaningful when on libcxx.";
#endif
}

/// @test With thread safety analysis support and not on libc++, `arene::base::lock_guard<T>` is an annotated
/// wrapper over `std::lock_guard<T>`
TEST(LockGuard, IsWrapperIfTsaIsSupportedAndNotOnLibcxx) {
#if ARENE_IS_ON(ARENE_HAS_THREAD_SAFETY_ANALYSIS) && ARENE_IS_OFF(ARENE_STDLIB_LIBCXX)
  ::testing::StaticAssertTypeEq<lock_guard<std::mutex>, arene::base::mutex_detail::lock_guard<std::mutex> >();
#else
  GTEST_SKIP() << "Only meaningful when TSA is enabled an we're not on libcxx.";
#endif
}

}  // namespace
