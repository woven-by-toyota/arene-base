// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file timed_mutex.hpp
/// @brief Provides a thin wrapper around std::timed_mutex for enabling TSA
///

// IWYU pragma: private
// IWYU pragma: friend "arene/base/mutex/.*"

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_TIMED_MUTEX_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_TIMED_MUTEX_HPP_

#include <chrono>
#include <mutex>
#include <type_traits>

#include "arene/base/compiler_support/thread_safety_annotations.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/mutex/detail/has_native_handle.hpp"

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {
namespace mutex_detail {

///
/// @brief A thin wrapper around a timed mutex which annotates it as a _capability_ for Thread Safety Analysis.
/// @tparam Mutex the mutex to wrap. Defaults to @c std::timed_mutex. Must satisfy the _TimedMutex_ concept.
///
template <typename Mutex = std::timed_mutex>
class ARENE_TSA_CAPABILITY("arene::base::timed_mutex") timed_mutex
    : mutex_detail::mutex_native_handle_member_mixin<timed_mutex<Mutex>> {
 public:
  /// @brief Construct a mutex in an unlocked state
  constexpr timed_mutex() = default;
  /// @brief Destroy the mutex
  ~timed_mutex() = default;
  /// @brief Not copyable
  timed_mutex(timed_mutex const&) = delete;
  /// @brief Not movable
  timed_mutex(timed_mutex&&) = delete;
  /// @brief Not copyable
  auto operator=(timed_mutex const&) -> timed_mutex& = delete;
  /// @brief Not movable
  auto operator=(timed_mutex&&) -> timed_mutex& = delete;

  /// @brief Passthrough to Mutex::lock()
  /// @pre The calling thread must not have already _acquired_ the _capability_, otherwise it will be a thread safety
  ///      analysis compiler diagnostic.
  /// @post The calling thread will have _acquired_ the _capability_.
  void lock() ARENE_TSA_ACQUIRE() { mtx_.lock(); }

  /// @brief Passthrough to Mutex::try_lock()
  /// @post The calling thread will be conditionally assumed to have _acquired_ the _capability_. Please see the
  ///       \ref compiler_support_thread_safety_annotations manual for more information.
  /// @return @c true if the lock was acquired, @c false otherwise
  ARENE_TSA_TRY_ACQUIRE(true) auto try_lock() -> bool { return mtx_.try_lock(); }

  /// @brief Passthrough to Mutex::try_lock_for<Rep, Period>()
  /// @param timeout_duration minimum duration to block for
  /// @post The calling thread will be conditionally assumed to have _acquired_ the _capability_. Please see the
  ///       \ref compiler_support_thread_safety_annotations manual for more information.
  /// @return @c true if the lock was acquired, @c false otherwise
  template <typename Rep, typename Period>
  ARENE_TSA_TRY_ACQUIRE(true)
  auto try_lock_for(std::chrono::duration<Rep, Period> const& timeout_duration) -> bool {
    return mtx_.try_lock_for(timeout_duration);
  }

  /// @brief Passthrough to Mutex::try_lock_until<Clock, Duration>()
  /// @param timeout_time maximum time point to block until
  /// @post The calling thread will be conditionally assumed to have _acquired_ the _capability_. Please see the
  ///       \ref compiler_support_thread_safety_annotations manual for more information.
  /// @return @c true if the lock was acquired, @c false otherwise
  template <typename Clock, typename Duration>
  ARENE_TSA_TRY_ACQUIRE(true)
  auto try_lock_until(std::chrono::time_point<Clock, Duration> const& timeout_time) -> bool {
    return mtx_.try_lock_until(timeout_time);
  }

  /// @brief Passthrough to Mutex::unlock()
  /// @post The calling thread will have _released_ the _capability_.
  void unlock() ARENE_TSA_RELEASE() { return mtx_.unlock(); }

  /// @brief Passthrough to Mutex::native_handle(), if the underlying mutex supports it.
  /// @return The native handle
  template <typename M = Mutex, constraints<std::enable_if_t<mutex_detail::has_native_handle<M>>> = nullptr>
  auto native_handle() -> decltype(auto) {
    return mtx_.native_handle();
  }

 private:
  /// @brief The wrapped mutex
  Mutex mtx_;
};

/// @brief If this file has been included, then the selected timed_mutex is the wrapper mutex
using selected_timed_mutex = ::arene::base::mutex_detail::timed_mutex<std::timed_mutex>;

}  // namespace mutex_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_TIMED_MUTEX_HPP_
