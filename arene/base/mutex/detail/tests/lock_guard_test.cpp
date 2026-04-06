// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mutex/detail/lock_guard.hpp"

#include <mutex>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/mutex/detail/mutex.hpp"
#include "arene/base/mutex/detail/tests/mock_mutex.hpp"

namespace {

using ::arene::base::mutex_detail::lock_guard;
using ::arene::base::mutex_detail::mutex;
using ::arene::base::mutex_test::mock_mutex;

/// @test Constructing a `lock_guard` locks the supplied mutex exactly once
TEST(LockGuard, LocksMutexExactlyOnceInCtor) {
  mock_mutex mtx;
  EXPECT_CALL(mtx, lock).Times(1);
  lock_guard<mock_mutex> const guard(mtx);
}

/// @test Destroying a `lock_guard` unlocks the mutex supplied to the constructor exactly once
TEST(LockGuard, UnlocksMutexExactlyOnceInDtor) {
  mock_mutex mtx;
  EXPECT_CALL(mtx, unlock).Times(1);
  {  // mtx will be unlocked after this scope
    lock_guard<mock_mutex> const guard(mtx);
  }
}

/// @test If Thread Safety Analysis is enabled, `lock_guard` is declared as a "scoped capability"
TEST(LockGuard, IsAScopedCapability) {
  // This has to be done as conditional compilation as compilers see ARENE_IS_OFF as an unconditional answer.
#if ARENE_IS_OFF(ARENE_HAS_THREAD_SAFETY_ANALYSIS)
  GTEST_SKIP() << "Only meaningful under TSA";
#else
  struct data {
    arene::base::mutex_detail::mutex<> mtx;
    int value ARENE_TSA_GUARDED_BY(mtx) = 0;
  };
  data test;
  lock_guard<decltype(test.mtx)> const guard(test.mtx);
  // this read/write pair doesn't matter, we just have to touch the data
  test.value = 1;
  ASSERT_EQ(test.value, 1);
#endif
}

/// @test `selected_lock_guard` aliases for `lock_guard`
TEST(SelectedLockGuard, IsLockGuardWrapper) {
  ::testing::StaticAssertTypeEq<::arene::base::mutex_detail::selected_lock_guard<std::mutex>, lock_guard<std::mutex>>();
}

}  // namespace
