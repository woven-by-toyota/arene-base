// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mutex/mutex.hpp"

#include <mutex>

#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"

namespace {

using ::arene::base::mutex;

TEST(LockGuard, IsStdLockGuardIfTsaIsNotSupported) {
#if ARENE_IS_OFF(ARENE_HAS_THREAD_SAFETY_ANALYSIS)
  ::testing::StaticAssertTypeEq<mutex, std::mutex>();
#else
  GTEST_SKIP() << "Only meaningful when TSA is disabled.";
#endif
}

TEST(LockGuard, IsStdLockGuardIfOnLibcxx) {
#if ARENE_IS_ON(ARENE_STDLIB_LIBCXX)
  ::testing::StaticAssertTypeEq<mutex, std::mutex>();
#else
  GTEST_SKIP() << "Only meaningful when on libcxx.";
#endif
}

TEST(LockGuard, IsWrapperIfTsaIsSupportedAndNotOnLibcxx) {
#if ARENE_IS_ON(ARENE_HAS_THREAD_SAFETY_ANALYSIS) && ARENE_IS_OFF(ARENE_STDLIB_LIBCXX)
  ::testing::StaticAssertTypeEq<mutex, ::arene::base::mutex_detail::mutex<std::mutex>>();
#else
  GTEST_SKIP() << "Only meaningful when TSA is enabled an we're not on libcxx.";
#endif
}

}  // namespace
