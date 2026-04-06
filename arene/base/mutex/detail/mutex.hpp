// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file mutex.hpp
/// @brief Provides a thin wrapper around std::mutex for enabling TSA
///

// IWYU pragma: private
// IWYU pragma: friend "arene/base/mutex/.*"

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_MUTEX_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_MUTEX_HPP_

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
/// @brief A thin wrapper around a mutex which annotates it as a _capability_ for Thread Safety Analysis.
/// @tparam Mutex the mutex to wrap. Defaults to @c std::mutex. Must satisfy the _Mutex_ concept.
///
template <typename Mutex = std::mutex>
class ARENE_TSA_CAPABILITY("arene::base::mutex") mutex : mutex_detail::mutex_native_handle_member_mixin<mutex<Mutex>> {
 public:
  /// @brief Construct in an unlocked state
  constexpr mutex() = default;
  /// @brief Destroy the mutex
  ~mutex() = default;
  /// @brief Not copyable
  mutex(mutex const&) = delete;
  /// @brief Not movable
  mutex(mutex&&) = delete;
  /// @brief Not copyable
  auto operator=(mutex const&) -> mutex& = delete;
  /// @brief Not movable
  auto operator=(mutex&&) -> mutex& = delete;

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

  /// @brief Passthrough to Mutex::unlock()
  /// @post The calling thread will have _released_ the _capability_.
  void unlock() ARENE_TSA_RELEASE() { return mtx_.unlock(); }

  /// @brief Passthrough to Mutex::native_handle(), if the underlying mutex supports it.
  /// @return The native handle
  template <typename M = Mutex, constraints<std::enable_if_t<has_native_handle<M>>> = nullptr>
  auto native_handle() -> decltype(auto) {
    return mtx_.native_handle();
  }

 private:
  /// @brief The wrapped mutex
  Mutex mtx_;
};

/// @brief If this file has been included, then the selected mutex is the wrapper mutex
using selected_mutex = ::arene::base::mutex_detail::mutex<std::mutex>;

}  // namespace mutex_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_MUTEX_HPP_
