// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_TESTS_MOCK_MUTEX_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_TESTS_MOCK_MUTEX_HPP_

#include <chrono>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace arene {
namespace base {
namespace mutex_test {

///
/// @brief A simple mock class that exposes the full Mutex/TimedMutex named capability.
/// @{
class mutex_interface {
 public:
  virtual ~mutex_interface() = default;
  virtual void lock() = 0;
  virtual auto try_lock() -> bool = 0;
  virtual void unlock() = 0;

  using time_point = std::chrono::steady_clock::time_point;
  using duration = time_point::duration;
  virtual auto try_lock_for(duration const&) -> bool = 0;
  virtual auto try_lock_until(time_point const&) -> bool = 0;

 protected:
  mutex_interface() = default;
  mutex_interface(mutex_interface const&) = default;
  mutex_interface(mutex_interface&&) = default;
  auto operator=(mutex_interface const&) -> mutex_interface& = default;
  auto operator=(mutex_interface&&) -> mutex_interface& = default;
};

class mock_mutex : public mutex_interface {
 public:
  // NOLINTBEGIN(modernize-use-trailing-return-type) clang-tidy sees into these macros, and google doesn't use TR.
  MOCK_METHOD(void, lock, (), (override));
  MOCK_METHOD(bool, try_lock, (), (override));
  MOCK_METHOD(void, unlock, (), (override));

  MOCK_METHOD(bool, try_lock_for, (mutex_interface::duration const&), (override));
  MOCK_METHOD(bool, try_lock_until, (mutex_interface::time_point const&), (override));
  // NOLINTEND(modernize-use-trailing-return-type))

  /// We exploit the native_handle API to let us get at the mock mutex and set expectations.
  /// @{
  using native_handle_t = mock_mutex&;
  auto native_handle() -> native_handle_t { return *this; }
  /// @}
};
/// @}

}  // namespace mutex_test
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_TESTS_MOCK_MUTEX_HPP_
