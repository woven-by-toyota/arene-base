// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::inner_product"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_INNER_PRODUCT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_INNER_PRODUCT_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/detail/generalized_transform_reduce.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/multiplies.hpp"
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
namespace inner_product_detail {

/// @brief function object implementing 'inner_product'
class inner_product_fn {
 public:
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
  ///   <c> acc = plus(std::move(acc), multiplies(*i1, *i2)) </c> for every iterator
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
  /// @note Unlike @c transform_reduce, @c inner_product always performs the
  ///   operations in the order given.
  ///
  template <
      class InputIt1,
      class InputIt2,
      class Value,
      class Plus,
      class Multiplies,
      constraints<
          std::enable_if_t<is_input_iterator_v<InputIt1>>,
          std::enable_if_t<is_input_iterator_v<InputIt2>>,
          std::enable_if_t<is_copyable_v<Value>>,
          std::enable_if_t<
            std::is_assignable<
              Value&,
              invoke_result_t<
                Plus&,
                Value&&,
                invoke_result_t<
                  Multiplies&,
                  iter_reference_t<InputIt1>,
                  iter_reference_t<InputIt2>
                >
              >
            >::value>> = nullptr>
  constexpr auto operator()(
      InputIt1 first1,
      InputIt1 last1,
      InputIt2 first2,
      Value init,
      Plus plus,
      Multiplies multiplies
  ) const noexcept( //
          std::is_nothrow_move_constructible<Value>::value &&
          is_nothrow_invocable_v<
              generalized_transform_reduce,
              generalized_transform_reduce::default_reduce_value_policy<Value>,
              InputIt1&,
              InputIt1&,
              InputIt2&,
              Value,
              Plus&,
              Multiplies&>
      )
      -> Value
  {
    using policy = generalized_transform_reduce::default_reduce_value_policy<Value>;

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
  ///   <c> acc = std::move(acc) + (*i1 * *i2)) </c> for every iterator
  ///   @c i1 in the range <c> [first1, last1) </c> in order and its
  ///   corresponding iterator @c i2 in the range beginning at @c first2.
  ///
  /// @pre <c> [first2, first2 + distance(first1, last1)) </c> is a valid range
  /// @pre binary @c operator+ does not invalidate subranges, nor modify
  ///   elements in the ranges <c> [first1, last1) </c> and
  ///   <c> [first2, first2 + distance(first1, last1)) </c>
  /// @pre binary @c operator* does not invalidate subranges, nor modify
  ///   elements in the ranges <c> [first1, last1) </c> and
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
  /// @note Unlike @c transform_reduce, @c inner_product always performs the
  ///   operations in the order given.
  ///
  template <
      class InputIt1,
      class InputIt2,
      class Value,
      constraints<
          std::enable_if_t<is_input_iterator_v<InputIt1>>,
          std::enable_if_t<is_input_iterator_v<InputIt2>>,
          std::enable_if_t<is_copyable_v<Value>>,
          std::enable_if_t<std::is_assignable<
              Value&,
              invoke_result_t<
                  std::plus<>&,
                  Value&&,
                  invoke_result_t<  //
                      std::multiplies<>&,
                      iter_reference_t<InputIt1>,
                      iter_reference_t<InputIt2>>>>::value>> = nullptr>
  constexpr auto operator()(  //
      InputIt1 first1,
      InputIt1 last1,
      InputIt2 first2,
      Value init
  ) const
      noexcept(is_nothrow_invocable_v<
               inner_product_fn,
               InputIt1&&,
               InputIt1&&,
               InputIt2&&,
               Value&&,
               std::plus<>&&,
               std::multiplies<>&&>) -> Value {
    return (*this)(
        std::move(first1),
        std::move(last1),
        std::move(first2),
        std::move(init),
        std::plus<>{},
        std::multiplies<>{}
    );
  }
};
}  // namespace inner_product_detail
}  // namespace algorithm_detail

/// @def arene::base::inner_product
/// @brief computes the inner product of two ranges of elements
/// @copydoc arene::base::algorithm_detail::inner_product_detail::inner_product_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(algorithm_detail::inner_product_detail::inner_product_fn, inner_product);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_INNER_PRODUCT_HPP_
