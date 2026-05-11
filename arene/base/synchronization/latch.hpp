// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SYNCHRONIZATION_LATCH_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SYNCHRONIZATION_LATCH_HPP_

#include <atomic>
#include <cstddef>
#include <limits>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/expect.hpp"
#include "arene/base/contracts.hpp"
#include "arene/base/synchronization/manual_reset_event.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

/// @brief Backport of @c std::latch from C++20. Provides a count-down latch that stays
/// permanently signalled when the count reaches zero.  For a given instance,
/// every call to @c count_down or @c arrive_and_wait synchronizes-with every
/// call to @c wait or @c arrive_and_wait, and every call to @c try_wait that
/// returns @c true.
class latch {
 public:
  /// @brief Get the maximum supported count
  /// @return The maximum count
  static constexpr auto max() noexcept -> std::ptrdiff_t { return std::numeric_limits<std::ptrdiff_t>::max(); }

  // parasoft-begin-suppress AUTOSAR-A3_1_5-a-2 "False positive: intended to be inlined"
  /// @brief Construct a latch with the specified count. If the @c count is zero, then
  /// the latch will be already signalled.
  /// @param initial_count The initial count for the latch
  /// @pre initial_count >= 0
  constexpr explicit latch(std::ptrdiff_t initial_count)
      : count_(initial_count) {
    ARENE_PRECONDITION(initial_count >= 0);
  }
  // parasoft-end-suppress AUTOSAR-A3_1_5-a-2

  /// @brief Default destructor
  ~latch() = default;

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Not copyable
  latch(latch const&) = delete;
  /// @brief Not copyable
  auto operator=(latch const&) -> latch& = delete;
  /// @brief Not movable
  latch(latch&&) = delete;
  /// @brief Not movable
  auto operator=(latch&&) -> latch& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief Check if the latch is signalled without blocking.
  /// @return @c true if the latch is signalled, @c false otherwise.
  auto try_wait() const noexcept -> bool { return count_.load(std::memory_order_acquire) == 0; }

  // parasoft-begin-suppress AUTOSAR-A3_1_5-a-2 "False positive: intended to be inlined"
  /// @brief Count down the latch by @c update. If the counter reaches zero, the latch
  /// is permanently signalled, and any threads blocked in @c wait are
  /// unblocked.
  /// @param update The amount by which to decrease the count
  /// @pre <c>(update >= 0) && (update <= count_)</c>
  /// @throw std::system_error on error
  void count_down(std::ptrdiff_t update) {
    ARENE_PRECONDITION((update >= 0) && (update <= count_));
    if (count_.fetch_sub(update, std::memory_order_acq_rel) == update) {
      event_.signal();
    }
  }
  // parasoft-end-suppress AUTOSAR-A3_1_5-a-2

  /// @brief Count down the latch by 1. If the counter reaches zero, the latch
  /// is permanently signalled, and any threads blocked in @c wait are
  /// unblocked.
  /// @pre The latch must not be signalled.
  /// @throw std::system_error on error
  void count_down() { count_down(1); }

  /// @brief Blocks the current thread until the latch becomes signalled.
  /// @throw std::system_error on error
  void wait() const {
    if (!try_wait()) {
      event_.wait();
    }
  }

  /// @brief Count down the latch by @c update and blocks the current thread waiting
  /// for the latch to be signalled.
  /// @param update The amount by which to decrease the count
  /// @throw std::system_error on error
  void arrive_and_wait(std::ptrdiff_t const update) {
    count_down(update);
    wait();
  }

  /// @brief Count down the latch by 1 and blocks the current thread waiting
  /// for the latch to be signalled.
  /// @throw std::system_error on error
  void arrive_and_wait() { arrive_and_wait(1); }

 private:
  /// @brief The remaining count
  std::atomic<std::ptrdiff_t> count_;
  /// @brief An event used for signalling
  manual_reset_event event_;
};

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SYNCHRONIZATION_LATCH_HPP_
