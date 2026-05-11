// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::iota"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_IOTA_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_IOTA_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// IWYU pragma: no_include "arene/base/stdlib_choice/iterator_traits.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {

namespace iota_detail {

/// @brief function object providing the implementation of 'iota'
class iota_impl_fn {
 public:
  /// @brief fills a range with successive increments of the starting value
  /// @tparam ForwardIt the type of the ForwardIt
  /// @tparam Value the type of the value
  /// @param first ForwardIt to the beginning of the range
  /// @param last ForwardIt to one past the end of the range
  /// @param value initial value to fill with
  ///
  /// Private implementation shared by @c arene::base::iota and @c std::iota.
  /// This function takes arguments by lvalue reference to avoid requiring
  /// @c Value to be movable.
  ///
  template <
      class ForwardIt,
      class Value,
      constraints<
          std::enable_if_t<is_forward_iterator_v<ForwardIt>>,
          std::enable_if_t<std::is_convertible<Value, algorithm_detail::iter_value_t<ForwardIt>>::value>,
          decltype(++std::declval<Value&>())> = nullptr>
  constexpr auto operator()(ForwardIt& first, ForwardIt& last, Value& value) const noexcept(  //
      noexcept(std::declval<ForwardIt&>() != std::declval<ForwardIt&>()) &&                   //
      noexcept(*std::declval<ForwardIt&>()++ = std::declval<Value&>()) &&                     //
      noexcept(++std::declval<Value&>())                                                      //
  )                                                                                           //
      -> void                                                                                 //
  {
    while (first != last) {
      // parasoft-begin-suppress AUTOSAR-M5_2_10-a "idiomatic iterator operations permitted by M5-2-10 Permit #1"
      *first++ = value;
      // parasoft-end-suppress AUTOSAR-M5_2_10-a

      // parasoft-begin-suppress AUTOSAR-M4_5_3-a "This function must work with all supported incrementable types"
      // parasoft-begin-suppress AUTOSAR-M5_0_15-a "'value' is required to be an incrementable type
      ++value;
      // parasoft-end-suppress AUTOSAR-M4_5_3-a
      // parasoft-end-suppress AUTOSAR-M5_0_15-a
    }
  }
};

/// @brief function object implementing 'iota'
class iota_fn {
 public:
  /// @brief fills a range with successive increments of the starting value
  /// @tparam ForwardIt the type of the ForwardIt
  /// @tparam Value the type of the value
  /// @param first ForwardIt to the beginning of the range
  /// @param last ForwardIt to one past the end of the range
  /// @param value initial value to fill with
  ///
  /// Fills the range <c> [first, last) </c> with sequentially increasing
  /// values, starting with @c value and repetitively evaluating
  /// <c> ++value </c>.
  ///
  /// @pre @c ForwardIt must satisfy the forward iterator requirements
  /// @pre @c T is convertible to the value type of @c ForwardIt
  /// @pre the expression <c> ++value </c> is well-formed
  ///
  /// @note Complexity <br>
  ///   Exactly <c> distance(first, last) </c> increments and assignments.
  ///
  template <
      class ForwardIt,
      class Value,
      constraints<
          std::enable_if_t<is_forward_iterator_v<ForwardIt>>,
          std::enable_if_t<std::is_convertible<Value, algorithm_detail::iter_value_t<ForwardIt>>::value>,
          decltype(++std::declval<Value&>())> = nullptr>
  constexpr auto operator()(ForwardIt first, ForwardIt last, Value value) const noexcept(  //
      arene::base::is_nothrow_invocable_v<iota_impl_fn, ForwardIt&, ForwardIt&, Value&>    //
  )                                                                                        //
      -> void                                                                              //
  {
    iota_impl_fn{}(first, last, value);
  }
};
}  // namespace iota_detail

/// @def arene::base::iota
/// @brief fills a range with successive increments of the starting value
/// @copydoc arene::base::iota_detail::iota_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(iota_detail::iota_fn, iota);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_IOTA_HPP_
