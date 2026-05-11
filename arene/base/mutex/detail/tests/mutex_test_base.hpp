// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_TESTS_MUTEX_TEST_BASE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_TESTS_MUTEX_TEST_BASE_HPP_

#include <type_traits>
#include <utility>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/compiler_support/thread_safety_annotations.hpp"
#include "arene/base/mutex/detail/tests/mock_mutex.hpp"

namespace arene {
namespace base {
namespace mutex_test {

using ::testing::Eq;
using ::testing::Return;

/// @return true if MutexType::native_handle() returns mock_mutex&, else false.
template <typename MutexType>
constexpr bool native_handle_is_mock_mutex_v =
    std::is_same<decltype(std::declval<MutexType>().native_handle()), mock_mutex&>::value;

///
/// @brief A typed test suite validating the basic mutex API is correct and a Capability.
///
/// Tests lock(), unlock(), and try_lock().
///
/// @tparam MutexType Must be a Mutex whose native_handle API returns mock_mutex&
///
template <class MutexType>
class MutexBaseTest : public ::testing::Test {
  static_assert(native_handle_is_mock_mutex_v<MutexType>, "only valid for mutexes whose native handle is mock_mutex");
};

TYPED_TEST_SUITE_P(MutexBaseTest);

/// @test If Thread Safety Analysis is enabled, the mutex type is marked as a capability
/// @tparam TypeParam The type of the mutex
TYPED_TEST_P(MutexBaseTest, IsACapability) {
  // This has to be done as conditional compilation as compilers see ARENE_IS_OFF as an unconditional answer.
#if ARENE_IS_OFF(ARENE_HAS_THREAD_SAFETY_ANALYSIS)
  GTEST_SKIP() << "Only meaningful under TSA";
#else
  // Here, if you were to delete any of the calls to lock() or unlock() this should result in compiler errors.
  struct data {
    TypeParam mtx;
    int value ARENE_TSA_GUARDED_BY(mtx) = 0;
  };
  data test;
  test.mtx.lock();
  // this read/write pair doesn't matter, we just have to touch the data
  test.value = 1;
  EXPECT_EQ(test.value, 1);
  test.mtx.unlock();

  if (test.mtx.try_lock()) {
    // this read/write pair doesn't matter, we just have to touch the data
    test.value = 2;
    EXPECT_EQ(test.value, 2);
    test.mtx.unlock();
  }
#endif
}

/// @test Locking and unlocking a wrapper mutex locks and unlocks the native handle
/// @tparam TypeParam The type of the mutex
TYPED_TEST_P(MutexBaseTest, LockAndUnlockCallUnderlyingMutexApis) {
  // we have to do both in one go because TSA will not let us leave the mutex locked.
  TypeParam mtx;
  EXPECT_CALL(mtx.native_handle(), lock).Times(1);
  EXPECT_CALL(mtx.native_handle(), unlock).Times(1);
  mtx.lock();
  mtx.unlock();
}

/// @test Invoking `try_lock` wrapper mutex invokes `try_lock` on the wrapped mutex and returns the result
/// @tparam TypeParam The type of the mutex
TYPED_TEST_P(MutexBaseTest, TryLockCallsUnderlyingMutexApiAndPassesThroughReturn) {
  TypeParam mtx;

  // We expect try_lock() to be called twice; the first time it returns false, the second time true.
  EXPECT_CALL(mtx.native_handle(), try_lock()).Times(2).WillOnce(Return(false)).WillOnce(Return(true));

  // We have to do this in a slightly weird way, because TSA doesn't actually handle runtime branch conditional locking,
  // it just uses path analysis to validate that the lock is released in the success branch.
  if (mtx.try_lock()) {
    // the first call doesn't return success, so we fail if we get here, but TSA doesn't know that so needs to see the
    // mutex unlocked.
    mtx.unlock();
    GTEST_FAIL() << "Should have returned false for the first call";
  }
  if (!mtx.try_lock()) {
    // The second call should return true, so we fail if it didn't
    GTEST_FAIL() << "Should have returned true for the second call";
  }
  mtx.unlock();
}

REGISTER_TYPED_TEST_SUITE_P(
    MutexBaseTest,
    IsACapability,
    LockAndUnlockCallUnderlyingMutexApis,
    TryLockCallsUnderlyingMutexApiAndPassesThroughReturn
);
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(MutexBaseTest);

}  // namespace mutex_test
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_TESTS_MUTEX_TEST_BASE_HPP_
