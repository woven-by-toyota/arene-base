// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ATOMIC_ATOMIC_MONOTONIC_COUNTER_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ATOMIC_ATOMIC_MONOTONIC_COUNTER_HPP_

// IWYU pragma: private, include "arene/base/atomic.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

/// @brief A thread-safe atomic counter
///
/// A 64-bit unsigned monotonic counter using compiler atomic intrinsics, intended for use when the C++ standard library
/// atomic header is not available.
///
/// Note: All operations use sequentially consistent memory ordering.
// parasoft-begin-suppress AUTOSAR-A12_1_5-a "False positive: there is no common overlap between the constructors"
class atomic_monotonic_counter {
  // parasoft-begin-suppress CERT_C-PRE31-c "False positive: static_assert is a compile-time assert and can have no
  // side-effects"
  static_assert(
      ARENE_IS_ON(ARENE_HAS_BUILTIN_ATOMIC),
      "arene::base::atomic_monotonic_counter requires compiler builtin atomic operations"
  );
  // parasoft-end-suppress CERT_C-PRE31-c

 public:
  /// @brief The value type of the counter
  using value_type = std::uint64_t;

  /// @brief Default constructor initializing counter to 0
  constexpr atomic_monotonic_counter() noexcept
      : value_{} {}

  /// @brief Constructor with initial value
  /// @param initial_value The initial value for the counter
  constexpr explicit atomic_monotonic_counter(value_type const initial_value) noexcept
      : value_{initial_value} {}

  // parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
  /// @brief Pre-increment operator
  /// @return The value of the counter after modification
  ///
  /// Note: This returns a copy of the counter value, not a reference to the counter.
  auto operator++() noexcept -> value_type { return __atomic_add_fetch(&value_, value_type{1}, __ATOMIC_SEQ_CST); }

  /// @brief Post-increment operator
  /// @return The value before incrementing
  ///
  /// Note: This returns a copy of the counter value, not a reference to the counter.
  // parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: postincrement requires an int parameter"
  auto operator++(int) noexcept -> value_type { return __atomic_fetch_add(&value_, value_type{1}, __ATOMIC_SEQ_CST); }
  // parasoft-end-suppress AUTOSAR-A3_9_1-b-2

  /// @brief Atomically loads and returns the current counter value
  /// @return The current counter value
  auto load() const noexcept -> value_type { return __atomic_load_n(&value_, __ATOMIC_SEQ_CST); }

  /// @brief Implicit conversion to the counter's value type
  /// @return The current counter value
  // parasoft-begin-suppress AUTOSAR-A13_5_2-a "Implicit conversion is intended to match the std::atomic API."
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  operator value_type() const noexcept { return load(); }
  // parasoft-end-suppress AUTOSAR-A13_5_2-a

  // parasoft-end-suppress AUTOSAR-A7_1_5-a-2

 private:
  /// @brief The underlying counter value
  value_type value_;
};

// parasoft-end-suppress AUTOSAR-A12_1_5-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ATOMIC_ATOMIC_MONOTONIC_COUNTER_HPP_
