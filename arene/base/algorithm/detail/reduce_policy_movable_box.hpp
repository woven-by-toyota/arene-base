// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_REDUCE_POLICY_MOVABLE_BOX_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_REDUCE_POLICY_MOVABLE_BOX_HPP_

#include "arene/base/algorithm/detail/reduce_policy_move_assign.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_destructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/move.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace algorithm_detail {

/// @brief A reduce policy for use with @c arene::base::algorithm_detail::generalized_transform_reduce modeled on the
/// ranges @c movable-box utility to allow use of move-constructible but not move-assignable types with
/// @c transform_reduce or @c reduce
/// @tparam Value accumulator value type
///
/// Primary template for types that are *not* move assignable. Wraps the accumulator in an @c optional so updates can be
/// performed via @c emplace (destroy + in-place construct) instead of move assignment. This enables use of
/// @c transform_reduce or @c reduce with types that are move constructible but not move assignable.
///
/// When @c Value is move assignable, the specialization below avoids the @c optional wrapper and uses
/// @c reduce_policy_move_assign instead.
template <class Value, bool = std::is_move_assignable<Value>::value>
class reduce_policy_movable_box {
 public:
  // parasoft-begin-suppress AUTOSAR-A2_10_1-d "False positive: identifier 'value_type' does not hide anything"
  /// @brief value type to use in the underlying implementation function
  using value_type = optional<Value>;
  // parasoft-end-suppress AUTOSAR-A2_10_1-d

  /// @brief Assigns a new value to the reduced value.
  /// @tparam Result result type
  /// @param val reference to the value to assign to
  /// @param result value to assign
  ///
  /// Update @c val with @c result. This is intended to provide functionality
  /// similar to ranges exposition-only utility @c movable-box .
  ///
  /// @note optional::emplace is not constexpr.
  template <class Result>
  static auto assign(value_type& val, Result&& result) noexcept(
      std::is_nothrow_destructible<value_type>::value &&  //
      std::is_nothrow_constructible<Value, Result&&>::value
  ) -> void {
    val.emplace(std::forward<Result>(result));
  }

  /// @brief Accesses the current reduced value. Access _always_ moves the value.
  /// @param val rvalue reference to the reduced value
  /// @return <c> move(val) </c>
  static auto access(value_type& val) noexcept -> Value&& { return *std::move(val); }
  /// @brief Accesses the current reduced value. Access _always_ moves the value.
  /// @param val rvalue reference to the reduced value
  /// @return <c> move(val) </c>
  static auto access(value_type&& val) noexcept -> Value&& { return *std::move(val); }
};

/// @brief specialization of @c reduce_policy_movable_box for move-assignable
///   types
/// @tparam Value accumulator value type
///
/// Specialization for types that *are* move assignable.
template <class Value>
class reduce_policy_movable_box<Value, true>  //
    : public reduce_policy_move_assign<Value> {};

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_REDUCE_POLICY_MOVABLE_BOX_HPP_
