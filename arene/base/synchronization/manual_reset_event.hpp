// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SYNCHRONIZATION_MANUAL_RESET_EVENT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SYNCHRONIZATION_MANUAL_RESET_EVENT_HPP_

#include <semaphore.h>

#include <atomic>
#include <cerrno>
#include <mutex>
#include <system_error>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-A15_4_5-a-2 "Confusing std::__1::system_error and std::system_error"
// parasoft-begin-suppress AUTOSAR-A17_1_1-a-2 "False positive: this function encapsulates the ues of errno"
// parasoft-begin-suppress AUTOSAR-M19_3_1-a-2 "Errno used to report errors that cannot be checked in advance"
/// @brief Throw @c std::system_error based on the value of @c errno
/// @throw std::system_error
ARENE_NORETURN inline void throw_errno_as_system_error() { throw std::system_error(errno, std::system_category()); }
// parasoft-end-suppress AUTOSAR-M19_3_1-a-2
// parasoft-end-suppress AUTOSAR-A17_1-1-a-2
// parasoft-end-suppress AUTOSAR-A15_4_5-a-2

/// @brief An event class that allows blocking waits from an arbitrary number of
/// threads that are then woken when the event is signalled. Additional calls to
/// @c wait while the event is signalled will return immediately. If there are
/// no waiting threads, the event can be reset to an unsignalled state.
class manual_reset_event {
 public:
  /// @brief Initialize the event to an unsignalled state
  constexpr manual_reset_event() noexcept = default;

  /// @brief Destroy the event; destroy the semaphore if it is initialized.
  ~manual_reset_event() {
    if (initialized_) {
      static_cast<void>(sem_destroy(&sem_));
    }
  }
  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Not copyable
  manual_reset_event(manual_reset_event const&) = delete;
  /// @brief Not copyable
  auto operator=(manual_reset_event const&) -> manual_reset_event& = delete;
  /// @brief Not movable
  manual_reset_event(manual_reset_event&&) = delete;
  /// @brief Not movable
  auto operator=(manual_reset_event&&) -> manual_reset_event& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief Signal the event and wake any blocked waiters.
  /// @throw std::system_error on error.
  void signal() {
    if (!signalled_.exchange(true, std::memory_order_release)) {
      ensure_semaphore_initialized();
      post_semaphore();
    }
  }

  /// @brief Reset the event to an unsignalled state.
  /// @pre There must be no threads blocked in @c wait.
  /// @throw std::system_error on error.
  void reset() {
    if (signalled_.load(std::memory_order_acquire)) {
      signalled_.store(false, std::memory_order_release);
      wait_on_semaphore();
    }
  }

  /// @brief Wait for the event to be signalled. Return immediately if it is already
  /// signalled.
  /// @throw std::system_error on error.
  void wait() const;

 private:
  /// @brief Wait for the semaphore;
  /// @throw std::system_error on error.
  void wait_on_semaphore() const {
    if (sem_wait(&sem_) != 0) {
      throw_errno_as_system_error();
    }
  }

  /// @brief Post the semaphore;
  /// @throw std::system_error on error.
  void post_semaphore() const {
    if (sem_post(&sem_) != 0) {
      throw_errno_as_system_error();
    }
  }

  /// @brief Ensure the semaphore is initialized. Can safely be called from multiple
  /// threads concurrently. Does not return until the semaphore is initialized.
  void ensure_semaphore_initialized() const {
    std::call_once(init_flag_, [this]() {
      if (sem_init(&sem_, 0, 0) != 0) {
        throw_errno_as_system_error();
      }
      initialized_ = true;
    });
  }

  /// @brief The signalling state of the event
  std::atomic<bool> signalled_{false};
  /// @brief Is the semaphore initialized?
  mutable bool initialized_{false};
  /// @brief Flag to ensure the semaphore is initialized exactly once
  mutable std::once_flag init_flag_{};
  /// @brief The semaphore
  mutable sem_t sem_{};
};

/// @brief Wait for the event to be signalled. Return immediately if it is already
/// signalled.
/// @throw std::system_error if an error occurs in the underlying OS facilities
inline void manual_reset_event::wait() const {
  if (!signalled_.load(std::memory_order_acquire)) {
    ensure_semaphore_initialized();
    if (!signalled_.load(std::memory_order_acquire)) {
      wait_on_semaphore();
      post_semaphore();
    }
  }
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SYNCHRONIZATION_MANUAL_RESET_EVENT_HPP_
