// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines std::accumulate"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ACCUMULATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ACCUMULATE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

// IWYU pragma: private, include <numeric>
// IWYU pragma: friend "stdlib_detail/.*"
//
#include "arene/base/algorithm/detail/generalized_reduce.hpp"
#include "arene/base/algorithm/detail/reduce_policy_copy_assign.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/type_traits/is_copyable.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"

namespace std {
// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief computes a left fold of a range
/// @tparam InputIt type of the iterator for the range
/// @tparam Value type of the accumulated value
/// @tparam Plus type of the addition operation
/// @param first iterator to the beginning of the range
/// @param last iterator to one past the end of the range
/// @param init initial value to start with
/// @param plus binary function object used to reduce transformed elements
///   to a single value
///
/// @return Initializes the accumulator @c acc (of type @c Value) with the
///   initial value @c init and then modifies it with the expression
///   <c> acc = plus(acc, *it) </c> for every iterator
///   @c it in the range <c> [first, last) </c> in order.
///
/// @pre @c plus does not invalidate subranges, nor modify elements in the
///   range <c> [first, last) </c>
///
/// @note Constraints <br>
///   * @c InputIt must satisfy the input iterator requirements
///   * @c Value is copy constructible
///   * @c Value is copy assignable
///   * <c> plus(init, *first) </c> is assignable to @c Value
///
/// @note Complexity <br>
///   O(N) applications of @c plus, where
///   <c> N = distance(first, last) </c>.
///
/// @note @c accumulate always performs the operations in the order given.
///
template <
    class InputIt,
    class Value,
    class Plus,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_input_iterator_v<InputIt>>,
        std::enable_if_t<arene::base::is_copyable_v<Value>>,
        std::enable_if_t<
          std::is_assignable<
            Value&,
            arene::base::invoke_result_t<
              Plus&,
              Value&,
              arene::base::algorithm_detail::iter_reference_t<InputIt>
            >
          >::value
        >
    > = nullptr>
auto accumulate(
    InputIt first,
    InputIt last,
    Value init,
    Plus plus
) noexcept( //
        std::is_nothrow_move_constructible<Value>::value &&
        arene::base::is_nothrow_invocable_v<
            arene::base::algorithm_detail::generalized_reduce,
            arene::base::algorithm_detail::reduce_policy_copy_assign<Value>,
            InputIt&,
            InputIt&,
            Value,
            Plus&
            >
    )
    -> Value
{
  using arene::base::algorithm_detail::generalized_reduce;
  using policy = arene::base::algorithm_detail::reduce_policy_copy_assign<Value>;

  return policy::access(     //
      generalized_reduce{}(  //
          policy{},
          first,
          last,
          std::move(init),
          plus
      )
  );
}

/// @brief computes a left fold of a range
/// @tparam InputIt type of the iterator for the range
/// @tparam Value type of the accumulated value
/// @param first iterator to the beginning of the range
/// @param last iterator to one past the end of the range
/// @param init initial value to start with
///
/// @return Initializes the accumulator @c acc (of type @c Value) with the
///   initial value @c init and then modifies it with the expression
///   <c> acc = acc + *it </c> for every iterator
///   @c it in the range <c> [first, last) </c> in order.
///
/// @pre binary @c operator+ does not invalidate subranges, nor modify elements
///   in the range <c> [first, last) </c>
///
/// @note Constraints <br>
///   * @c InputIt must satisfy the input iterator requirements
///   * @c Value is copy constructible
///   * @c Value is copy assignable
///   * <c> init + *first </c> is assignable to @c Value
///
/// @note Complexity <br>
///   O(N) applications of @c operator+, where
///   <c> N = distance(first, last) </c>.
///
/// @note @c accumulate always performs the operations in the order given.
///
template <
    class InputIt,
    class Value,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_input_iterator_v<InputIt>>,
        std::enable_if_t<arene::base::is_copyable_v<Value>>,
        std::enable_if_t<std::is_assignable<
            Value&,
            arene::base::invoke_result_t<  //
                plus<>&,
                Value&,
                arene::base::algorithm_detail::iter_reference_t<InputIt>>>::value>> = nullptr>
auto accumulate(
    InputIt first,
    InputIt last,
    Value init
) noexcept(  //
    noexcept(std::accumulate(
        std::declval<InputIt&&>(),
        std::declval<InputIt&&>(),
        std::declval<Value&&>(),
        std::declval<std::plus<>&&>()
    ))
) -> Value {
  return std::accumulate(std::move(first), std::move(last), std::move(init), std::plus<>{});
}

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_ACCUMULATE_HPP_
