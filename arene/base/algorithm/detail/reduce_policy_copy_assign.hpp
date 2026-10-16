// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_REDUCE_POLICY_COPY_ASSIGN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_REDUCE_POLICY_COPY_ASSIGN_HPP_

#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/move.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace algorithm_detail {

/// @brief A copying reduce policy for use with @c arene::base::algorithm_detail::generalized_transform_reduce
/// @tparam Value accumulator value type
///
/// This reduce value policy has an @c access function that does *not* @c std::move the value, it simply provides a
/// reference. The @c assign function is equivalent to @c operator=.
///
/// The policy @c value is simply @c Value unchanged.
template <class Value>
class reduce_policy_copy_assign {
 public:
  /// @brief value type to use in the underlying implementation function
  using value_type = Value;

  // parasoft-begin-suppress AUTOSAR-A8_4_8-a "'val' is a reference to the assigned value and must be passed in as a
  // parameter"
  /// @brief generalized assignment
  /// @tparam Result result type
  /// @param val reference to the value to assign to
  /// @param result value to assign
  ///
  /// Update @c val with @c result with the @c value_type assignment
  /// operation.
  template <class Result>
  static constexpr auto assign(value_type& val, Result&& result) noexcept(
      std::is_nothrow_assignable<Value&, Result&&>::value
  ) -> void {
    val = std::forward<Result>(result);
  }
  // parasoft-end-suppress AUTOSAR-A8_4_8-a

  /// @brief generalized access
  /// @param val lvalue reference to the value
  /// @return <c> val </c>
  static constexpr auto access(value_type& val) noexcept -> Value& { return val; }
  /// @brief generalized access
  /// @param val rvalue reference to the value
  /// @return <c> move(val) </c>
  static constexpr auto access(value_type&& val) noexcept -> Value&& { return std::move(val); }
};

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_REDUCE_POLICY_COPY_ASSIGN_HPP_
