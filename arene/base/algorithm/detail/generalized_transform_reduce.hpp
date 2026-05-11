// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_GENERALIZED_TRANSFORM_REDUCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_GENERALIZED_TRANSFORM_REDUCE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/algorithm/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/stdlib_choice/move.hpp"

namespace arene {
namespace base {
namespace algorithm_detail {

/// @brief function object providing the implementation of @c transform_reduce
///   and @c inner_product
class generalized_transform_reduce {
 public:
  /// @brief default policy for handling the accumulator
  /// @tparam Value accumulator value type
  ///
  /// This default policy has an @c access function equivalent to @c std::move
  /// and an @c assign function equivalent to @c operator=.
  ///
  /// The policy @c value is simply @c Value unchanged.
  ///
  template <class Value>
  class default_reduce_value_policy {
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
    template <class Result>
    static constexpr auto assign(value_type& acc, Result&& result) noexcept(
        std::is_nothrow_assignable<Value&, Result&&>::value
    ) -> void {
      acc = std::forward<Result>(result);
    }
    /// @brief generalized access
    /// @param acc lvalue reference to the accumulator
    /// @return <c> move(acc) </c>
    ///
    static constexpr auto access(value_type& acc) noexcept -> Value&& { return std::move(acc); }
    /// @brief generalized access
    /// @param acc rvalue reference to the accumulator
    /// @return <c> move(acc) </c>
    ///
    static constexpr auto access(value_type&& acc) noexcept -> Value&& { return std::move(acc); }
  };

  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  /// @brief applies an invocable, then reduces out of order
  /// @tparam ReducePolicy type specifying access and assignment of the accumulator
  /// @tparam InputIt1 type of the iterator for the first range
  /// @tparam InputIt2 type of the iterator for the second range
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
  /// @note This a generalized algorithm intended for use by @c transform_reduce
  ///   and @c inner_product
  ///
  /// @note This implementation currently always reduces *in* order.
  ///
  template <
      class ReducePolicy,
      class InputIt1,
      class InputIt2,
      class Reduce,
      class Transform,
      class Value = typename ReducePolicy::value_type>
  constexpr auto operator()(  //
      ReducePolicy,
      InputIt1& first1,
      InputIt1& last1,
      InputIt2& first2,
      typename ReducePolicy::value_type init,
      Reduce& reduce,
      Transform& transform
  ) const noexcept(                                                        //
      noexcept(std::declval<InputIt1&>() != std::declval<InputIt1&>()) &&  //
      noexcept(ReducePolicy::assign(                                       //
          std::declval<Value&>(),                                          //
          std::declval<Reduce&>()(                                         //
              ReducePolicy::access(std::declval<Value&>()),                //
              std::declval<Transform&>()(                                  //
                  *std::declval<InputIt1>(),                               //
                  *std::declval<InputIt2>()                                //
              )                                                            //
          )                                                                //
      )) &&                                                                //
      noexcept(++std::declval<InputIt1&>()) &&                             //
      noexcept(++std::declval<InputIt2&>())                                //
  )                                                                        //
      -> Value                                                             //
  {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(
        noexcept(ReducePolicy::access(std::declval<Value&>())),  //
        "'ReducePolicy::access' must always be 'noexcept' as it simply returns a reference"
    );
    // parasoft-end-suppress CERT_C-PRE31-c-3

    while (first1 != last1) {
      ReducePolicy::assign(  //
          init,
          reduce(ReducePolicy::access(init), transform(*first1, *first2))
      );
      ++first1;
      ++first2;
    }

    return init;
  }
  // parasoft-end-suppress CERT_C-EXP37-a
};

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_GENERALIZED_TRANSFORM_REDUCE_HPP_
