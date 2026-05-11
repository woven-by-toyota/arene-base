// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/mutex/detail/timed_mutex.hpp"

#include <chrono>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/mutex/detail/tests/mock_mutex.hpp"  // IWYU pragma: keep
#include "arene/base/mutex/detail/tests/mutex_test_base.hpp"

// Note: INSTANTIATE_TYPED_TEST_SUITE_P can't handle FQNs, but also needs ADL to work, so you have to instantiate the
// test in the same namespace as the test suite was declared in. Simple imports aren't enough, it has to actually be in
// the same namespace.
namespace arene {
namespace base {
namespace mutex_test {

using ::arene::base::mutex_detail::timed_mutex;
using ::arene::base::mutex_test::mock_mutex;

// A timed mutex is a mutex
INSTANTIATE_TYPED_TEST_SUITE_P(TimedMutex, MutexBaseTest, timed_mutex<mock_mutex>, );

/// @test Invoking `try_lock_for` on a wrapper mutex passes the duration to `try_lock_for` on the wrapped mutex
TEST(TimedMutexTimedTryLocks, TryLockForPassesInputDurationToMutex) {
  timed_mutex<mock_mutex> mtx;
  mock_mutex::duration const expected{123};
  EXPECT_CALL(mtx.native_handle(), try_lock_for(Eq(expected))).Times(1);
  if (mtx.try_lock_for(expected)) {
    // it doesn't actually matter if this runs, we just need TSA to be happy.
    mtx.unlock();
  }
}

/// @test Invoking `try_lock_for` on a wrapper mutex returns the result of calling `try_lock_for` on the wrapped mutex
TEST(TimedMutexTimedTryLocks, TryLockForReturnsValueFromMutex) {
  timed_mutex<mock_mutex> mtx;

  // We expect try_lock_for() to be called twice; the first time it returns false, the second time true.
  EXPECT_CALL(mtx.native_handle(), try_lock_for).Times(2).WillOnce(Return(false)).WillOnce(Return(true));

  // We have to do this in a slightly weird way, because TSA doesn't actually handle runtime branch conditional locking,
  // it just uses path analysis to validate that the lock is released in the success branch.
  if (mtx.try_lock_for(mock_mutex::duration{123})) {
    // the first call doesn't return success, so we fail if we get here, but TSA doesn't know that so needs to see the
    // mutex unlocked.
    mtx.unlock();
    GTEST_FAIL() << "Should have returned false for the first call";
  }
  if (!mtx.try_lock_for(mock_mutex::duration{123})) {
    // The second call should return true, so we fail if it didn't
    GTEST_FAIL() << "Should have returned true for the second call";
  }
  mtx.unlock();
}

/// @test Invoking `try_lock_until` on a wrapper mutex passes the time point to `try_lock_until` on the wrapped mutex
TEST(TimedMutexTimedTryLocks, TryLockUntilPassesInputTimePointToMutex) {
  timed_mutex<mock_mutex> mtx;
  auto const expected = mock_mutex::time_point::clock::now();
  EXPECT_CALL(mtx.native_handle(), try_lock_until(Eq(expected))).Times(1);
  if (mtx.try_lock_until(expected)) {
    // it doesn't actually matter if this runs, we just need TSA to be happy.
    mtx.unlock();
  }
}

/// @test Invoking `try_lock_until` on a wrapper mutex returns the result of calling `try_lock_until` on the wrapped
/// mutex
TEST(TimedMutexTimedTryLocks, TryLockUntilReturnsValueFromMutex) {
  timed_mutex<mock_mutex> mtx;

  // We expect try_lock_for() to be called twice; the first time it returns false, the second time true.
  EXPECT_CALL(mtx.native_handle(), try_lock_until).Times(2).WillOnce(Return(false)).WillOnce(Return(true));

  // We have to do this in a slightly weird way, because TSA doesn't actually handle runtime branch conditional locking,
  // it just uses path analysis to validate that the lock is released in the success branch.
  if (mtx.try_lock_until(mock_mutex::time_point::clock::now())) {
    // the first call doesn't return success, so we fail if we get here, but TSA doesn't know that so needs to see the
    // mutex unlocked.
    mtx.unlock();
    GTEST_FAIL() << "Should have returned false for the first call";
  }
  if (!mtx.try_lock_until(mock_mutex::time_point::clock::now())) {
    // The second call should return true, so we fail if it didn't
    GTEST_FAIL() << "Should have returned true for the second call";
  }
  mtx.unlock();
}

/// @test `selected_timed_mutex` is `timed_mutex<std::timed_mutex>`
TEST(SelectedTimedMutex, IsTimedMutexWrapper) {
  ::testing::StaticAssertTypeEq<::arene::base::mutex_detail::selected_timed_mutex, timed_mutex<std::timed_mutex>>();
}

}  // namespace mutex_test
}  // namespace base
}  // namespace arene
