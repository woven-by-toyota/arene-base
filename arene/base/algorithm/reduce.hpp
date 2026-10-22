// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::reduce"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_REDUCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_REDUCE_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/detail/generalized_reduce.hpp"
#include "arene/base/algorithm/detail/reduce_policy_movable_box.hpp"
#include "arene/base/algorithm/detail/synthesize_range1_overload.hpp"  // IWYU pragma: export
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_constructible.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/stdlib_choice/plus.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// IWYU pragma: no_include "arene/base/stdlib_choice/iterator_traits.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {
namespace algorithm_detail {
namespace reduce_detail {

/// @brief function object implementing 'reduce'
class reduce_fn {
 public:
  /// @brief performs a fold with an unspecified order
  /// @tparam InputIt type of the iterator for the range
  /// @tparam Value type of the reduced value
  /// @tparam Reduce type of the reduction operation
  /// @param first iterator to the beginning of the range
  /// @param last iterator to one past the end of the range
  /// @param init initial value to start with
  /// @param reduce binary function object used to reduce transformed elements
  ///   to a single value
  ///
  /// @return The generalized sum of @c init and @c values over @c reduce.
  ///
  /// @pre @c reduce does not invalidate subranges, nor modify
  ///   elements in the range <c> [first, last) </c>
  ///
  /// @note Constraints <br>
  ///   * @c InputIt must satisfy the input iterator requirements
  ///   * @c Value is move constructible
  ///   * <c> reduce(init, init) </c> is convertible to @c Value
  ///   * <c> reduce(init, *first)  </c> is convertible to @c Value
  ///   * <c> reduce(*first, init) </c> is convertible to @c Value
  ///   * <c> reduce(*first, *first) </c> is convertible to @c Value
  ///
  /// @note Complexity <br>
  ///   O(N) applications of @c reduce , where
  ///   <c> N = distance(first1, last1) </c>.
  ///
  /// @note Unlike @c accumulate, the order in which elements are reduced
  ///   is unspecified.
  ///
  /// @note The result is non-deterministic if @c reduce is not associative or
  ///   not commutative (e.g. floating-point addition).
  ///
  template <
      class InputIt,
      class Value,
      class Reduce,
      constraints<
          std::enable_if_t<is_input_iterator_v<InputIt>>,
          std::enable_if_t<std::is_move_constructible<Value>::value>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              Reduce&,
              Value&,
              iter_reference_t<InputIt> >>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              Reduce&,
              iter_reference_t<InputIt>,
              Value& >>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              Reduce&,
              Value&,
              Value& >>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              Reduce&,
              iter_reference_t<InputIt>,
              iter_reference_t<InputIt> >>> = nullptr>
  constexpr auto operator()(
      InputIt first,
      InputIt last,
      Value init,
      Reduce reduce
  ) const noexcept( //
          std::is_nothrow_constructible<
            typename reduce_policy_movable_box<Value>::value_type,
            Value&&
          >::value &&
          std::is_nothrow_move_constructible<Value>::value &&
          is_nothrow_invocable_v<
              generalized_reduce,
              reduce_policy_movable_box<Value>,
              InputIt&,
              InputIt&,
              typename reduce_policy_movable_box<Value>::value_type,
              Reduce& >
      )
      -> Value
  {
    using value_policy = reduce_policy_movable_box<Value>;

    return value_policy::access(  //
        generalized_reduce{}(     //
            value_policy{},
            first,
            last,
            typename value_policy::value_type{std::move(init)},
            reduce
        )
    );
  }

  /// @brief performs a fold with an unspecified order
  /// @tparam InputIt type of the iterator for the range
  /// @tparam Value type of the reduced value
  /// @param first iterator to the beginning of the range
  /// @param last iterator to one past the end of the range
  /// @param init initial value to start with
  ///
  /// @return The generalized sum of @c init and @c values over @c operator+.
  ///
  /// @pre binary @c operator+ does not invalidate subranges, nor modify
  ///   elements in the range <c> [first, last) </c>
  ///
  /// @note Constraints <br>
  ///   * @c InputIt must satisfy the input iterator requirements
  ///   * @c Value is move constructible
  ///   * <c> init + init </c> is convertible to @c Value
  ///   * <c> init + *first  </c> is convertible to @c Value
  ///   * <c> *first + init </c> is convertible to @c Value
  ///   * <c> *first + *first </c> is convertible to @c Value
  ///
  /// @note Complexity <br>
  ///   O(N) applications of @c operator+ , where
  ///   <c> N = distance(first, last) </c>.
  ///
  /// @note Unlike @c accumulate, the order in which elements are reduced
  ///   is unspecified.
  ///
  /// @note The result is non-deterministic if @c operator+ is not associative or
  ///   not commutative (e.g. floating-point addition).
  ///
  template <
      class InputIt,
      class Value,
      constraints<
          std::enable_if_t<is_input_iterator_v<InputIt>>,
          std::enable_if_t<std::is_move_constructible<Value>::value>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              std::plus<>&,
              Value&,
              iter_reference_t<InputIt> >>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              std::plus<>&,
              iter_reference_t<InputIt>,
              Value& >>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              std::plus<>&,
              Value&,
              Value& >>,
          std::enable_if_t<is_invocable_r_v<  //
              Value,
              std::plus<>&,
              iter_reference_t<InputIt>,
              iter_reference_t<InputIt> >>> = nullptr>
  constexpr auto operator()(
      InputIt first,
      InputIt last,
      Value init
  ) const noexcept( //
          is_nothrow_invocable_v<
              reduce_fn,
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
}  // namespace reduce_detail
}  // namespace algorithm_detail

/// @brief performs a fold with an unspecified order
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress AUTOSAR-M3_4_1-b "This is a per-TU reference to a global used in multiple TUs"
// parasoft-begin-suppress CERT_CPP-DCL56-a "False positive: variable is initialized"
ARENE_CPP14_INLINE_VARIABLE(
    algorithm_detail::synthesize_range1_overload_t<algorithm_detail::reduce_detail::reduce_fn>,
    reduce
);
// parasoft-end-suppress CERT_CPP-DCL56-a
// parasoft-end-suppress AUTOSAR-M3_4_1-b
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_REDUCE_HPP_
