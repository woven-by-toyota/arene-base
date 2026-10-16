// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::accumulate"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_ACCUMULATE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_ACCUMULATE_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/detail/generalized_reduce.hpp"
#include "arene/base/algorithm/detail/reduce_policy_move_assign.hpp"
#include "arene/base/algorithm/detail/synthesize_range1_overload.hpp"  // IWYU pragma: export
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/type_traits/is_copyable.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// IWYU pragma: no_include "arene/base/stdlib_choice/iterator_traits.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {
namespace algorithm_detail {
namespace accumulate_detail {

/// @brief function object implementing 'accumulate'
class accumulate_fn {
 public:
  /// @brief computes a left fold of a range
  /// @tparam InputIt type of the iterator for the range
  /// @tparam Value type of the accumulated value
  /// @tparam Plus type of the addition operation
  /// @param first iterator to the beginning of the range
  /// @param last iterator to one past the end of the range
  /// @param init initial value to start with
  /// @param plus binary function object used to "add" elements to the accumulated value.
  ///
  /// @return Initializes the accumulator @c acc (of type @c Value) with the
  ///   initial value @c init and then modifies it with the expression
  ///   <c> acc = plus(std::move(acc), *it) </c> for every iterator
  ///   @c it in the range <c> [first, last) </c> in order.
  ///
  /// @pre @c plus does not invalidate subranges, nor modify elements in the
  ///   range <c> [first, last) </c>, else behavior is undefined.
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
  template <
      class InputIt,
      class Value,
      class Plus,
      arene::base::constraints<
          std::enable_if_t<is_input_iterator_v<InputIt>>,
          std::enable_if_t<is_copyable_v<Value>>,
          std::enable_if_t<
            std::is_assignable<
              Value&,
              invoke_result_t<
                Plus&,
                Value&&,
                iter_reference_t<InputIt>
              >
            >::value
          >
      > = nullptr>
  constexpr auto operator()(
      InputIt first,
      InputIt last,
      Value init,
      Plus plus
  ) const noexcept( //
          std::is_nothrow_move_constructible<Value>::value &&
          is_nothrow_invocable_v<
              generalized_reduce,
              reduce_policy_move_assign<Value>,
              InputIt&,
              InputIt&,
              Value,
              Plus& >
      )
      -> Value
  {
    using value_policy = reduce_policy_move_assign<Value>;

    return value_policy::access(  //
        generalized_reduce{}(     //
            value_policy{},
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
  ///   O(N) applications of @c operator+ , where
  ///   <c> N = distance(first, last) </c>.
  ///
  /// @note @c accumulate always performs the operations in the order given.
  template <
      class InputIt,
      class Value,
      constraints<
          std::enable_if_t<is_input_iterator_v<InputIt>>,
          std::enable_if_t<std::is_move_constructible<Value>::value>,
          std::enable_if_t<std::is_assignable<
              Value&,
              arene::base::invoke_result_t<  //
                  std::plus<>&,
                  Value&&,
                  iter_reference_t<InputIt>>>::value>> = nullptr>
  constexpr auto operator()(
      InputIt first,
      InputIt last,
      Value init
  ) const noexcept( //
          is_nothrow_invocable_v<
              accumulate_fn,
              InputIt&&,
              InputIt&&,
              Value&&,
              std::plus<>&&>
      )
      -> Value
  {
    return (*this)(  //
        std::move(first),
        std::move(last),
        std::move(init),
        std::plus<>{}
    );
  }
};
}  // namespace accumulate_detail
}  // namespace algorithm_detail

/// @brief performs a left fold of a range
/// Starting with the initial value, combines each element in order using the supplied binary operation.
/// Overloads accept either an iterator pair or a range.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variable is initialized"
ARENE_CPP14_INLINE_VARIABLE(
    algorithm_detail::synthesize_range1_overload_t<algorithm_detail::accumulate_detail::accumulate_fn>,
    accumulate
);
// parasoft-end-suppress CERT_CPP-DCL56-a
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_ACCUMULATE_HPP_
