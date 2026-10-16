// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_GENERALIZED_REDUCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_GENERALIZED_REDUCE_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/algorithm/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"
// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/forward.hpp"
#include "arene/base/utility/make_subrange.hpp"

namespace arene {
namespace base {
namespace algorithm_detail {

/// @brief function object providing the implementation of @c reduce
///   and @c accumulate
class generalized_reduce {
 public:
  // parasoft-begin-suppress CERT_C-EXP37-a "False positive: The rule does not mention naming all parameters"
  /// @brief performs a fold with an unspecified order
  /// @tparam ReducePolicy type specifying access and assignment of the accumulator
  /// @tparam InputIt type of the iterator for the range
  /// @tparam Reduce type of the reduction operation
  /// @param first iterator to the beginning of the range
  /// @param last iterator to one past the end of the range
  /// @param init initial value to start with
  /// @param reduce binary function object used to reduce elements
  ///   to a single value
  ///
  /// @return The generalized sum of @c init and @c values over @c reduce.
  ///
  /// @note This a generalized algorithm intended for use by @c reduce
  ///   and @c accumulate
  ///
  /// @note This implementation currently always reduces *in* order.
  template <
      class ReducePolicy,
      class InputIt,
      class Reduce,
      class Value = typename ReducePolicy::value_type>
  constexpr auto operator()(  //
      ReducePolicy,
      InputIt& first,
      InputIt& last,
      typename ReducePolicy::value_type init,
      Reduce& reduce
  ) const noexcept(                                                      //
      noexcept(std::declval<InputIt&>() != std::declval<InputIt&>()) &&  //
      noexcept(ReducePolicy::assign(                                     //
          std::declval<Value&>(),                                        //
          std::declval<Reduce&>()(                                       //
              ReducePolicy::access(std::declval<Value&>()),              //
              *std::declval<InputIt>()                                   //
          )                                                              //
      )) &&                                                              //
      noexcept(++std::declval<InputIt&>())                               //
  )                                                                      //
      -> Value                                                           //
  {
    // parasoft-begin-suppress CERT_C-PRE31-c-3 "False positive: static_assert is a compile-time assert and can have no
    // side-effects"
    static_assert(
        noexcept(ReducePolicy::access(std::declval<Value&>())),  //
        "'ReducePolicy::access' must always be 'noexcept' as it simply returns a reference"
    );
    // parasoft-end-suppress CERT_C-PRE31-c-3

    for (auto&& next : ::arene::base::make_subrange(first, last)) {
      ReducePolicy::assign(  //
          init,
          reduce(ReducePolicy::access(init), std::forward<decltype(next)>(next))
      );
    }

    return init;
  }
  // parasoft-end-suppress CERT_C-EXP37-a
};

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_GENERALIZED_REDUCE_HPP_
