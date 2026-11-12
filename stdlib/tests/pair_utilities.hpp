// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_TESTS_PAIR_UTILITIES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_TESTS_PAIR_UTILITIES_HPP_

#include "stdlib/include/cstdint"

namespace test {
namespace pair_utilities {

template <typename T>
constexpr auto constexpr_check(T&&) -> bool {
  return true;
}

struct throwable_copy_construction {
  constexpr throwable_copy_construction() = default;
  // NOLINTNEXTLINE(hicpp-use-equals-default)
  constexpr throwable_copy_construction(throwable_copy_construction const&) noexcept(false) {}
  constexpr throwable_copy_construction(throwable_copy_construction&&) noexcept(false) {}
  constexpr auto operator=(throwable_copy_construction const&) -> throwable_copy_construction& = default;
  constexpr auto operator=(throwable_copy_construction&&) -> throwable_copy_construction& = default;
  ~throwable_copy_construction() = default;
};

class construction_tracker {
 public:
  enum class state : std::uint8_t {
    default_constructed,
    copy_constructed,
    move_constructed,
    direct_constructed,
    direct_rvalue_constructed,
    copy_assigned,
    move_assigned,
    swapped_lhs,
    swapped_rhs
  };

  struct some_arg_type {};
  struct explicit_arg_type {};

  construction_tracker() noexcept
      : state_(state::default_constructed) {}
  construction_tracker(construction_tracker const&) noexcept
      : state_(state::copy_constructed) {}
  construction_tracker(construction_tracker&&) noexcept
      : state_(state::move_constructed) {}
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  construction_tracker(some_arg_type const&) noexcept
      : state_(state::direct_constructed) {}
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  construction_tracker(some_arg_type&&) noexcept
      : state_(state::direct_rvalue_constructed) {}

  auto operator=(construction_tracker&&) noexcept -> construction_tracker& {
    state_ = state::move_assigned;
    return *this;
  }
  auto operator=(construction_tracker const&) noexcept -> construction_tracker& {
    state_ = state::copy_assigned;
    return *this;
  }

  ~construction_tracker() = default;

  auto get_tracked_state() const noexcept -> state { return state_; }

  void swap(construction_tracker& other) noexcept {
    state_ = state::swapped_lhs;
    other.state_ = state::swapped_rhs;
  }

 private:
  state state_;
};

inline void swap(construction_tracker& lhs, construction_tracker& rhs) noexcept { lhs.swap(rhs); }

}  // namespace pair_utilities
}  // namespace test
#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_TESTS_PAIR_UTILITIES_HPP_
