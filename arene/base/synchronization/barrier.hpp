// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SYNCHRONIZATION_BARRIER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SYNCHRONIZATION_BARRIER_HPP_

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/compiler_support/expect.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/contracts.hpp"
#include "arene/base/synchronization/manual_reset_event.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

namespace barrier_detail {
/// @brief A default do-nothing completion function for barrier
class default_completion_function {
 public:
  // parasoft-begin-suppress AUTOSAR-M0_1_8-b-2 "This is intended to do nothing"
  /// @brief Do-nothing function call operator
  constexpr void operator()() const noexcept {}
  // parasoft-end-suppress AUTOSAR-M0_1_8-b-2
};
}  // namespace barrier_detail

/// @brief A backport of @c std::barrier from C++20.
/// @tparam CompletionFunction The type of the completion function. Defaults to
/// an invocable type with a do-nothing function call operator.
/// @pre @c CompletionFunction must be destructible, move-constructible and
/// nothrow-invocable.
template <typename CompletionFunction = barrier_detail::default_completion_function>
class barrier {
  static_assert(std::is_destructible<CompletionFunction>::value, "The completion function must be destructible");
  static_assert(
      std::is_move_constructible<CompletionFunction>::value,
      "The completion function must be move-constructible"
  );
  // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert"
  static_assert(
      noexcept(std::declval<CompletionFunction&>()),
      "The completion function must be nothrow-invocable with no arguments"
  );
  // parasoft-end-suppress CERT_C-PRE31-c-3

  /// @brief Special tag type to prevent user construction of @c arrival_token
  class private_construction_tag {
   public:
    /// @brief Disable aggregate initialization
    explicit private_construction_tag() noexcept = default;
  };

 public:
  /// @brief Get the maximum supported count
  /// @return The maximum count
  static constexpr auto max() noexcept -> std::ptrdiff_t { return std::numeric_limits<std::ptrdiff_t>::max(); }

  /// @brief A class holding the result of a call to @c arrive, which can then be
  /// passed to @c wait to wait for the end of the completion step associated
  /// with the call to @c arrive.
  class arrival_token {
    /// @brief The phase that the call to @c arrive is associated with
    std::uint64_t phase_;

   public:
    // parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
    /// @brief Construct a new arrival token for the phase.
    /// @param phase The phase number
    arrival_token(private_construction_tag, std::uint64_t phase) noexcept
        : phase_(phase) {}
    // parasoft-end-suppress AUTOSAR-M2_10_1-a-2

    /// @brief Default move constructor
    arrival_token(arrival_token&& other) noexcept = default;
    /// @brief Default move assignment
    auto operator=(arrival_token&& other) noexcept -> arrival_token& = default;
    /// @brief not copyable
    arrival_token(arrival_token const& other) = delete;
    /// @brief not copyable
    auto operator=(arrival_token const& other) -> arrival_token& = delete;

    /// @brief Default destructor
    ~arrival_token() = default;

    /// @brief Get the phase this token is associated with
    /// @return the phase
    auto get_phase() const noexcept -> std::uint64_t { return phase_; }
  };

  // parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
  /// @brief Construct a barrier with a specified count and completion function.
  /// @param count The initial number of arrivals required to complete each
  /// phase
  /// @param func The completion function to invoke at the end of each phase
  constexpr barrier(std::ptrdiff_t count, CompletionFunction func) noexcept(
      std::is_nothrow_move_constructible<CompletionFunction>::value
  )
      : completion_function_(std::move(func)),
        phase_(0),
        initial_count_(count),
        count_(count) {
    ARENE_PRECONDITION(count >= 0);
  }
  // parasoft-end-suppress AUTOSAR-M2_10_1-a-2

  // parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
  /// @brief Construct a barrier with a specified count and default-constructed
  /// completion function.
  /// @param count The initial number of arrivals required to complete each
  /// phase
  template <
      typename F = CompletionFunction,
      constraints<std::enable_if_t<std::is_default_constructible<F>::value>> = nullptr>
  explicit constexpr barrier(std::ptrdiff_t count) noexcept(std::is_nothrow_default_constructible<F>::value)
      : completion_function_(CompletionFunction{}),
        phase_(0),
        initial_count_(count),
        count_(count) {
    ARENE_PRECONDITION(count >= 0);
  }
  // parasoft-end-suppress AUTOSAR-M2_10_1-a-2

  /// @brief Default destructor
  ~barrier() = default;

  // parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
  /// @brief Not copyable
  barrier(barrier const&) = delete;
  /// @brief Not copyable
  auto operator=(barrier const&) -> barrier& = delete;
  /// @brief Not movable
  barrier(barrier&&) = delete;
  /// @brief Not movable
  auto operator=(barrier&&) -> barrier& = delete;
  // parasoft-end-suppress CERT_C-EXP37-a-3

  /// @brief Arrive at the barrier and wait for the completion step to end.
  /// May trigger the completion step.
  /// @pre The remaining count for the current phase it at least one.
  void arrive_and_wait() { wait(arrive()); }

  /// @brief Decrease the number of arrivals required for the subsequent phases to end,
  /// and arrive at the barrier. May trigger the completion step. If the
  /// required number of arrivals is reduced to zero then the barrier has no
  /// more completion phases.
  /// @pre The remaining count for the current phase it at least one.
  void arrive_and_drop() {
    ARENE_PRECONDITION(initial_count_.load(std::memory_order_relaxed) > 0);
    std::ignore = initial_count_.fetch_sub(1, std::memory_order_relaxed);
    std::ignore = arrive();
  }

  /// @brief Arrive at the barrier with the specified number of arrivals, and obtain a
  /// token that can be used to wait for the end of the completion step
  /// associated with the current phase.
  /// @param arrivals The number of arrivals
  /// @return An @c arrival_token associated with the current phase
  /// @pre @c arrivals must be greater than zero, and less than or equal to the
  /// remaining number of arrivals required in the current phase.
  ARENE_NODISCARD auto arrive(std::ptrdiff_t arrivals) -> arrival_token {
    auto const remaining_count = count_.load(std::memory_order_relaxed);
    ARENE_PRECONDITION((arrivals > 0) && (arrivals <= remaining_count));
    auto phase = phase_.load(std::memory_order_relaxed);
    if (count_.fetch_sub(arrivals, std::memory_order_acq_rel) == arrivals) {
      run_completion_step(phase);
    }
    return {private_construction_tag{}, phase};
  }

  /// @brief Arrive at the barrier, and obtain a token that can be used to wait for the
  /// end of the completion step associated with the current phase.
  /// @return An @c arrival_token associated with the current phase
  ARENE_NODISCARD auto arrive() -> arrival_token { return arrive(1); }

  /// @brief Wait for the completion step associated with the phase identified by @c
  /// token to end.
  /// @param token A token returned from @c arrive
  /// @pre @c token must be associated with the current phase, or the previous
  /// one.
  void wait(arrival_token token) {
    auto const current_phase = phase_.load(std::memory_order_relaxed);
    ARENE_PRECONDITION((token.get_phase() == current_phase) || (token.get_phase() == current_phase - 1U));
    completed_event(token.get_phase()).wait();
  }

 private:
  /// @brief Run the completion step for the specified phase
  /// @param completed_phase the phase being completed
  void run_completion_step(std::uint64_t completed_phase) {
    completion_function_();
    auto new_phase = phase_.fetch_add(1U, std::memory_order_relaxed) + 1U;
    count_ = initial_count_.load(std::memory_order_relaxed);
    completed_event(new_phase).reset();
    completed_event(completed_phase).signal();
  }

  /// @brief Get the event associated with the specified phase
  /// @param event_phase the phase to get the event for
  /// @return A reference to the event for the specified phase
  auto completed_event(std::uint64_t event_phase) -> manual_reset_event& {
    constexpr std::uint64_t two{2U};
    return (event_phase % two) == 0U ? completed_even_ : completed_odd_;
  }

  /// @brief The completion function
  CompletionFunction completion_function_;
  /// @brief The current phase
  std::atomic<std::uint64_t> phase_;
  /// @brief The initial count of required arrivals for each phase
  std::atomic<std::ptrdiff_t> initial_count_;
  /// @brief The remaining count of required arrivals for the current phase
  std::atomic<std::ptrdiff_t> count_;
  /// @brief The completion event for odd numbered phases
  manual_reset_event completed_odd_;
  /// @brief The completion event for even numbered phases
  manual_reset_event completed_even_;
};
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_SYNCHRONIZATION_BARRIER_HPP_
