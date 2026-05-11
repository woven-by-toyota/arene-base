// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file lock_guard.hpp
/// @brief Provides a thin wrapper around std::lock_guard for enabling TSA
///

// IWYU pragma: private
// IWYU pragma: friend "arene/base/mutex/.*"

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_LOCK_GUARD_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_LOCK_GUARD_HPP_

#include <mutex>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/thread_safety_annotations.hpp"
#include "arene/base/contracts.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"

namespace arene {
namespace base {
namespace mutex_detail {

///
/// @brief A lock_guard implementation which annotates it as a _scoped capability_ for Thread Safety Analysis.
/// @tparam Mutex the mutex to guard. Must satisfy the _Mutex_ concept.
///
template <typename Mutex>
class ARENE_TSA_SCOPED_CAPABILITY lock_guard {
 public:
  /// @brief The type of the mutex being guarded.
  using mutex_type = Mutex;

  ///
  /// @brief Constructs the guard, and attempts to lock the input mutex.
  /// @param mtx The mutex to guard.
  /// @pre The calling thread must not have already _acquired_ the _capability_ provided by @c mtx , else a thread
  ///      safety analysis compiler diagnostic will be raised.
  /// @post The calling thread will have _acquired_ the _capability_ provided by @c mtx .
  ///
  explicit lock_guard(mutex_type& mtx) noexcept(noexcept(mtx.lock())) ARENE_TSA_ACQUIRE(mtx)
      : mtx_(&mtx) {
    mtx_->lock();
  }

  ///
  /// @brief Constructs the guard by adopting a mutex which is already locked.
  /// @param mtx The mutex to guard.
  /// @pre The calling thread must have already _acquired_ the _capability_ provided by @c mtx , else a thread
  ///      safety analysis compiler diagnostic will be raised.
  ///
  lock_guard(mutex_type& mtx, std::adopt_lock_t) noexcept ARENE_TSA_REQUIRES(mtx)
      : mtx_(&mtx) {}

  ///
  /// @brief Destroys the guard and unlocks the held mutex.
  /// @post The calling thread will have _released_ the guarded _capability_.
  ///
  ~lock_guard() noexcept ARENE_TSA_RELEASE() {
    ARENE_INVARIANT(mtx_ != nullptr);
    // parasoft-begin-suppress CERT_CPP-DCL57-a "If unlock throws, destructor's noexcept will catch and terminate"
    // parasoft-begin-suppress CERT_CPP-ERR50-b "If unlock throws, destructor's noexcept will catch and terminate"
    // parasoft-begin-suppress AUTOSAR-A15_5_1-a "If unlock throws, destructor's noexcept will catch and terminate"
    // parasoft-begin-suppress AUTOSAR-A15_5_3-b "If unlock throws, destructor's noexcept will catch and terminate"
    mtx_->unlock();
    // parasoft-end-suppress AUTOSAR-A15_5_3-b
    // parasoft-end-suppress AUTOSAR-A15_5_1-a
    // parasoft-end-suppress CERT_CPP-ERR50-b
    // parasoft-end-suppress CERT_CPP-DCL57-a
  }

  /// @brief Not copyable
  lock_guard(lock_guard const&) = delete;
  /// @brief Not movable
  lock_guard(lock_guard&&) = delete;
  /// @brief Not copyable
  auto operator=(lock_guard const&) -> lock_guard& = delete;
  /// @brief Not movable
  auto operator=(lock_guard&&) -> lock_guard& = delete;

 private:
  /// @brief A pointer to the mutex protected by the guard.
  /// @note Needs to be a pointer rather than a reference due to limitations in TSA's @c SCOPED_CAPABILITY
  ///       implementation.
  mutex_type* mtx_;
};

/// @brief If this file has been included, then the selected mutex is the annotated lock guard.
template <typename M>
using selected_lock_guard = ::arene::base::mutex_detail::lock_guard<M>;

}  // namespace mutex_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MUTEX_DETAIL_LOCK_GUARD_HPP_
