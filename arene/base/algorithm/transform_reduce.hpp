// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::transform_reduce"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TRANSFORM_REDUCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TRANSFORM_REDUCE_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/detail/generalized_transform_reduce.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/optional/optional.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_destructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/multiplies.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// IWYU pragma: no_include "arene/base/stdlib_choice/iterator_traits.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {
namespace algorithm_detail {
namespace transform_reduce_detail {

/// @brief function object implementing 'transform_reduce'
class transform_reduce_fn {
  /// @brief obtains the return type of invoking the first argument on a pack of dereferenced instances of the remaining
  /// arguments
  /// @tparam Invocable invocable type
  /// @tparam Iterator iterator argument types
  ///
  template <class Invocable, class... Iterator>
  using iter_result_t = invoke_result_t<Invocable, iter_reference_t<Iterator>...>;

  /// @brief policy for handling the accumulator
  /// @tparam Value accumulator value type
  ///
  /// Primary template for types that are *not* move assignable. Uses an
  /// @c optional to allow use of @c transform_reduce for types that are move
  /// constructible but not move assignable.
  ///
  template <class Value, bool = std::is_move_assignable<Value>::value>
  class reduce_value_policy {
   public:
    /// @brief value type to use in the underlying implementation function
    using value_type = optional<Value>;

    /// @brief generalized assignment
    /// @tparam Result result type
    /// @param acc reference to the accumulator
    /// @param result value to assign
    ///
    /// Update @c acc with @c result. This is intended to provide functionality
    /// similar to ranges exposition-only utility movable-box.
    ///
    template <class Result>
    static auto assign(value_type& acc, Result&& result) noexcept(
        std::is_nothrow_destructible<value_type>::value &&  //
        std::is_nothrow_constructible<Value, Result&&>::value
    ) -> void {
      acc.emplace(std::forward<Result>(result));
    }
    /// @brief generalized access
    /// @param acc lvalue reference to the accumulator
    /// @return <c> move(acc) </c>
    ///
    static auto access(value_type& acc) noexcept -> Value&& { return *std::move(acc); }
    /// @brief generalized access
    /// @param acc rvalue reference to the accumulator
    /// @return <c> move(acc) </c>
    ///
    static auto access(value_type&& acc) noexcept -> Value&& { return *std::move(acc); }
  };

  /// @brief policy for handling the accumulator
  /// @tparam Value accumulator value type
  ///
  /// Specialization for types that *are* move assignable.
  ///
  template <class Value>
  class reduce_value_policy<Value, true>  //
      : public generalized_transform_reduce::default_reduce_value_policy<Value> {};

 public:
  /// @brief applies an invocable, then reduces out of order
  /// @tparam InputIt1 type of the iterator for the first range
  /// @tparam InputIt2 type of the iterator for the second range
  /// @tparam Value type of the reduced value
  /// @tparam Reduce type of the reduction operation
  /// @tparam Transform type of the transform operation
  /// @param first1 iterator to the beginning of the first range
  /// @param last1 iterator to one past the end of the first range
  /// @param first2 iterator to the beginning of the second range
  /// @param init initial value to start with
  /// @param reduce binary function object used to reduce transformed elements
  ///   to a single value
  /// @param transform binary function object applied to each pair of elements
  ///   of the input ranges
  ///
  /// @return The generalized sum of @c init and @c values over @c reduce, where
  ///   @c values are the values transformed by @c transform, each value is
  ///   transformed from a pair of elements from the two input ranges.
  ///
  /// @pre <c> [first2, first2 + distance(first1, last1)) </c> is a valid range
  /// @pre @c reduce does not invalidate subranges, nor modify elements in the
  ///   ranges <c> [first1, last1) </c> and
  ///   <c> [first2, first2 + distance(first1, last1))) </c>
  /// @pre @c transform does not invalidate subranges, nor modify elements in the
  ///   ranges <c> [first1, last1) </c> and
  ///   <c> [first2, first2 + distance(first1, last1)) </c>
  ///
  /// @note Constraints <br>
  ///   * @c InputIt1 must satisfy the input iterator requirements
  ///   * @c InputIt2 must satisfy the input iterator requirements
  ///   * @c Value is move constructible
  ///   * <c> reduce(init, init) </c> is convertible to @c Value
  ///   * <c> reduce(init, transform(*first1, *first2)) </c>
  ///     is convertible to @c Value
  ///   * <c> reduce(transform(*first1, *first2), init) </c>
  ///     is convertible to @c Value
  ///   * <c> reduce(transform(*first1, *first2), transform(*first1, *first2)) </c>
  ///     is convertible to @c Value
  ///
  /// @note Complexity <br>
  ///   O(N) applications of @c reduce and @c transform, where
  ///   <c> N = distance(first1, last1) </c>.
  ///
  /// @note Unlike @c inner_product, the order in which elements are transformed
  ///   and reduced is unspecified.
  ///
  /// @note The result is non-deterministic if @c reduce is not associative or
  ///   not commutative (e.g. floating-point addition).
  ///
  template <
      class InputIt1,
      class InputIt2,
      class Value,
      class Reduce,
      class Transform,
      constraints<
          std::enable_if_t<is_input_iterator_v<InputIt1>>,
          std::enable_if_t<is_input_iterator_v<InputIt2>>,
          std::enable_if_t<std::is_move_constructible<Value>::value>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              Reduce&,
              Value&,
              Value&>>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              Reduce&,
              Value&,
              iter_result_t<Transform&, InputIt1, InputIt2>>>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              Reduce&,
              iter_result_t<Transform&, InputIt1, InputIt2>,
              Value&>>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              Reduce&,
              iter_result_t<Transform&, InputIt1, InputIt2>,
              iter_result_t<Transform&, InputIt1, InputIt2>>>> = nullptr>
  constexpr auto operator()(
      InputIt1 first1,
      InputIt1 last1,
      InputIt2 first2,
      Value init,
      Reduce reduce,
      Transform transform
  ) const noexcept( //
          std::is_nothrow_constructible<
            typename reduce_value_policy<Value>::value_type,
            Value&&
          >::value &&
          std::is_nothrow_move_constructible<Value>::value &&
          is_nothrow_invocable_v<
              generalized_transform_reduce,
              reduce_value_policy<Value>,
              InputIt1&,
              InputIt1&,
              InputIt2&,
              typename reduce_value_policy<Value>::value_type,
              Reduce&,
              Transform&>
      )
      -> Value
  {
    using value_policy = reduce_value_policy<Value>;

    return value_policy::access(         //
        generalized_transform_reduce{}(  //
            value_policy{},
            first1,
            last1,
            first2,
            typename value_policy::value_type{std::move(init)},
            reduce,
            transform
        )
    );
  }

  /// @brief applies an invocable, then reduces out of order
  /// @tparam InputIt1 type of the iterator for the first range
  /// @tparam InputIt2 type of the iterator for the second range
  /// @tparam Value type of the reduced value
  /// @param first1 iterator to the beginning of the first range
  /// @param last1 iterator to one past the end of the first range
  /// @param first2 iterator to the beginning of the second range
  /// @param init initial value to start with
  ///
  /// @return The generalized sum of @c init and @c values over @c operator+, where
  ///   @c values are the values transformed by @c operator*, each value is
  ///   transformed from a pair of elements from the two input ranges.
  ///
  /// @pre <c> [first2, first2 + distance(first1, last1)) </c> is a valid range
  /// @pre binary @c operator+ does not invalidate subranges, nor modify
  ///   elements in the ranges <c> [first1, last1) </c> and
  ///   <c> [first2, first2 + distance(first1, last1))) </c>
  /// @pre binary @c operator* does not invalidate subranges, nor modify
  ///   elements in the ranges <c> [first1, last1) </c> and
  ///   <c> [first2, first2 + distance(first1, last1)) </c>
  ///
  /// @note Constraints <br>
  ///   * @c InputIt1 must satisfy the input iterator requirements
  ///   * @c InputIt2 must satisfy the input iterator requirements
  ///   * @c Value is move constructible
  ///   * <c> init + init </c> is convertible to @c Value
  ///   * <c> init + (*first1 * *first2) </c>
  ///     is convertible to @c Value
  ///   * <c> (*first1 * *first2) + init </c>
  ///     is convertible to @c Value
  ///   * <c> (*first1 * *first2) + (*first1 * *first2)) </c>
  ///     is convertible to @c Value
  ///
  /// @note Complexity <br>
  ///   O(N) applications of @c operator+ and @c operator*, where
  ///   <c> N = distance(first1, last1) </c>.
  ///
  /// @note Unlike @c inner_product, the order in which elements are transformed
  ///   and reduced is unspecified.
  ///
  /// @note The result is non-deterministic if @c operator+ is not associative or
  ///   not commutative (e.g. floating-point addition).
  ///
  template <
      class InputIt1,
      class InputIt2,
      class Value,
      constraints<
          std::enable_if_t<is_input_iterator_v<InputIt1>>,
          std::enable_if_t<is_input_iterator_v<InputIt2>>,
          std::enable_if_t<std::is_move_constructible<Value>::value>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              std::plus<>&,
              Value&,
              Value&>>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              std::plus<>&,
              Value&,
              iter_result_t<std::multiplies<>&, InputIt1, InputIt2>>>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              std::plus<>&,
              iter_result_t<std::multiplies<>&, InputIt1, InputIt2>,
              Value&>>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              std::plus<>&,
              iter_result_t<std::multiplies<>&, InputIt1, InputIt2>,
              iter_result_t<std::multiplies<>&, InputIt1, InputIt2>>>> = nullptr>
  constexpr auto operator()(
      InputIt1 first1,
      InputIt1 last1,
      InputIt2 first2,
      Value init
  ) const noexcept( //
          is_nothrow_invocable_v<
              transform_reduce_fn,
              InputIt1&&,
              InputIt1&&,
              InputIt2&&,
              Value&&,
              std::plus<>&&,
              std::multiplies<>&&>
      )
      -> Value
  {
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
}  // namespace transform_reduce_detail
}  // namespace algorithm_detail

/// @def arene::base::transform_reduce
/// @brief applies an invocable, then reduces out of order
/// @copydoc arene::base::algorithm_detail::transform_reduce_detail::transform_reduce_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(algorithm_detail::transform_reduce_detail::transform_reduce_fn, transform_reduce);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_TRANSFORM_REDUCE_HPP_
