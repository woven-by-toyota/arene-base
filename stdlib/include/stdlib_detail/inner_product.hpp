// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines std::inner_product"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INNER_PRODUCT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INNER_PRODUCT_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

// IWYU pragma: private, include <numeric>
// IWYU pragma: friend "stdlib_detail/.*"
//
#include "arene/base/algorithm/detail/generalized_transform_reduce.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/multiplies.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/type_traits/is_copyable.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"

namespace std {
namespace inner_product_detail {

/// @brief reduce value policy for a C++14 compliant implementation
/// @tparam Value accumulator value type
///
/// This reduce value policy has an @c access function that does *not* @c
/// std::move the accumulator, it simply provides a reference.
/// The @c assign function is equivalent to @c operator=.
///
/// The policy @c value is simply @c Value unchanged.
template <class Value>
class reduce_value_policy {
 public:
  /// @brief value type to use in the underlying implementation function
  using value_type = Value;

  /// @brief generalized assignment
  /// @tparam Result result type
  /// @param acc reference to the accumulator
  /// @param result value to assign
  ///
  /// Update @c acc with @c result with the @c value_type assignment
  /// operation.
  ///
  // parasoft-begin-suppress AUTOSAR-A8_4_8-a "'acc' is a reference to the
  // assigned value and must be passed in as a parameter"
  template <class Result>
  static constexpr auto assign(value_type& acc, Result&& result) noexcept(
      std::is_nothrow_assignable<Value&, Result&&>::value
  ) -> void {
    acc = std::forward<Result>(result);
  }
  // parasoft-end-suppress AUTOSAR-A8_4_8-a

  /// @brief generalized access
  /// @param acc lvalue reference to the accumulator
  /// @return <c> acc </c>
  static constexpr auto access(value_type& acc) noexcept -> Value& { return acc; }
  /// @brief generalized access
  /// @param acc rvalue reference to the accumulator
  /// @return <c> move(acc) </c>
  static constexpr auto access(value_type&& acc) noexcept -> Value&& { return std::move(acc); }
};

}  // namespace inner_product_detail

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"
/// @brief computes the inner product of two ranges of elements
/// @tparam InputIt1 type of the iterator for the first range
/// @tparam InputIt2 type of the iterator for the second range
/// @tparam Value type of the accumulated value
/// @tparam Plus type of the addition operation
/// @tparam Multiplies type of the multiplication operation
/// @param first1 iterator to the beginning of the first range
/// @param last1 iterator to one past the end of the first range
/// @param first2 iterator to the beginning of the second range
/// @param init initial value to start with
/// @param plus binary function object used to reduce transformed elements
///   to a single value
/// @param multiplies binary function object applied to each pair of elements
///   of the input ranges
///
/// @return Initializes the accumulator @c acc (of type @c Value) with the
///   initial value @c init and then modifies it with the expression
///   <c> acc = plus(acc, multiplies(*i1, *i2)) </c> for every iterator
///   @c i1 in the range <c> [first1, last1) </c> in order and its
///   corresponding iterator @c i2 in the range beginning at @c first2.
///
/// @pre <c> [first2, first2 + distance(first1, last1)) </c> is a valid range
/// @pre @c plus does not invalidate subranges, nor modify elements in the
///   ranges <c> [first1, last1) </c> and
///   <c> [first2, first2 + distance(first1, last1)) </c>
/// @pre @c multiplies does not invalidate subranges, nor modify elements in the
///   ranges <c> [first1, last1) </c> and
///   <c> [first2, first2 + distance(first1, last1)) </c>
///
/// @note Constraints <br>
///   * @c InputIt1 must satisfy the input iterator requirements
///   * @c InputIt2 must satisfy the input iterator requirements
///   * @c Value is copy constructible
///   * @c Value is copy assignable
///   * <c> multiplies(*first1, *first2) </c> is a valid expression
///   * <c> plus(init, multiplies(*first1, *first2)) </c> is assignable
///     to @c Value
///
/// @note Complexity <br>
///   O(N) applications of @c plus and @c multiplies, where
///   <c> N = distance(first1, last1) </c>.
///
/// @note @c inner_product always performs the operations in the order given.
///
template <
    class InputIt1,
    class InputIt2,
    class Value,
    class Plus,
    class Multiplies,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_input_iterator_v<InputIt1>>,
        std::enable_if_t<arene::base::is_input_iterator_v<InputIt2>>,
        std::enable_if_t<arene::base::is_copyable_v<Value>>,
        std::enable_if_t<
          std::is_assignable<
            Value&,
            arene::base::invoke_result_t<
              Plus&,
              Value&,
              arene::base::invoke_result_t<
                Multiplies&,
                arene::base::algorithm_detail::iter_reference_t<InputIt1>,
                arene::base::algorithm_detail::iter_reference_t<InputIt2>
              >
            >
          >::value>> = nullptr>
auto inner_product(
    InputIt1 first1,
    InputIt1 last1,
    InputIt2 first2,
    Value init,
    Plus plus,
    Multiplies multiplies
) noexcept( //
        std::is_nothrow_move_constructible<Value>::value &&
        arene::base::is_nothrow_invocable_v<
            arene::base::algorithm_detail::generalized_transform_reduce,
            inner_product_detail::reduce_value_policy<Value>,
            InputIt1&,
            InputIt1&,
            InputIt2&,
            Value,
            Plus&,
            Multiplies&>
    )
    -> Value
{
  using arene::base::algorithm_detail::generalized_transform_reduce;
  using policy = inner_product_detail::reduce_value_policy<Value>;

  return policy::access(               //
      generalized_transform_reduce{}(  //
          policy{},
          first1,
          last1,
          first2,
          std::move(init),
          plus,
          multiplies
      )
  );
}

/// @brief computes the inner product of two ranges of elements
/// @tparam InputIt1 type of the iterator for the first range
/// @tparam InputIt2 type of the iterator for the second range
/// @tparam Value type of the accumulated value
/// @param first1 iterator to the beginning of the first range
/// @param last1 iterator to one past the end of the first range
/// @param first2 iterator to the beginning of the second range
/// @param init initial value to start with
///
/// @return Initializes the accumulator @c acc (of type @c Value) with the
///   initial value @c init and then modifies it with the expression
///   <c> acc = plus(acc, multiplies(*i1, *i2)) </c> for every iterator
///   @c i1 in the range <c> [first1, last1) </c> in order and its
///   corresponding iterator @c i2 in the range beginning at @c first2.
///
/// @pre <c> [first2, first2 + distance(first1, last1)) </c> is a valid range
/// @pre binary @c operator+ does not invalidate subranges, nor modify elements
///   in the ranges <c> [first1, last1) </c> and
///   <c> [first2, first2 + distance(first1, last1)) </c>
/// @pre binary @c operator* does not invalidate subranges, nor modify elements
///   in the ranges <c> [first1, last1) </c> and
///   <c> [first2, first2 + distance(first1, last1)) </c>
///
/// @note Constraints <br>
///   * @c InputIt1 must satisfy the input iterator requirements
///   * @c InputIt2 must satisfy the input iterator requirements
///   * @c Value is copy constructible
///   * @c Value is copy assignable
///   * <c> *first1 * *first2 </c> is a valid expression
///   * <c> init + multiplies(*first1, *first2) </c> is assignable
///     to @c Value
///
/// @note Complexity <br>
///   O(N) applications of @c plus and @c multiplies, where
///   <c> N = distance(first1, last1) </c>.
///
/// @note @c inner_product always performs the operations in the order given.
///
template <
    class InputIt1,
    class InputIt2,
    class Value,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_input_iterator_v<InputIt1>>,
        std::enable_if_t<arene::base::is_input_iterator_v<InputIt2>>,
        std::enable_if_t<arene::base::is_copyable_v<Value>>,
        std::enable_if_t<std::is_assignable<
            Value&,
            arene::base::invoke_result_t<
                plus<>&,
                Value&,
                arene::base::invoke_result_t<
                    multiplies<>&,
                    arene::base::algorithm_detail::iter_reference_t<InputIt1>,
                    arene::base::algorithm_detail::iter_reference_t<InputIt2>>>>::value>> = nullptr>
auto inner_product(
    InputIt1 first1,
    InputIt1 last1,
    InputIt2 first2,
    Value init
) noexcept(  //
    noexcept(std::inner_product(
        std::declval<InputIt1&&>(),
        std::declval<InputIt1&&>(),
        std::declval<InputIt2&&>(),
        std::declval<Value&&>(),
        std::declval<std::plus<>&&>(),
        std::declval<std::multiplies<>&&>()
    ))
) -> Value {
  return std::inner_product(
      std::move(first1),
      std::move(last1),
      std::move(first2),
      std::move(init),
      std::plus<>{},
      std::multiplies<>{}
  );
}

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INNER_PRODUCT_HPP_
