// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_ANY_OF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_ANY_OF_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/detail/convert_to.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/functional/identity.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/all_are_same.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_predicate.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/utility/make_subrange.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace any_of_detail {

/// @brief function object implementing 'any_of'
class any_of_fn  //
{
 public:
  /// @brief Check if any of the elements in a range are true
  /// @tparam Iterator the type of the iterator
  /// @tparam UnaryPredicate the type of the predicate
  /// @tparam Projection projection to apply to element in the range
  /// @param begin The start of the range
  /// @param end The end of the range
  /// @param pred The predicate to apply to each element of the range
  /// @param proj Projection to apply to elements before checking the predicate
  /// @return bool @c true if @c pred returns true for any of the projected
  ///   elements in the range, @c false otherwise.
  template <
      typename Iterator,
      typename UnaryPredicate,
      typename Projection = identity,
      constraints<
          std::enable_if_t<base::is_input_iterator_v<Iterator>>,
          std::enable_if_t<is_predicate_v<
              UnaryPredicate&,
              invoke_result_t<Projection&, algorithm_detail::iter_reference_t<Iterator>>>>> = nullptr>
  constexpr auto operator()(Iterator begin, Iterator end, UnaryPredicate pred, Projection proj = {}) const noexcept(  //
      noexcept(std::declval<Iterator&>() != std::declval<Iterator&>()) &&                                             //
      noexcept(pred(proj(*++std::declval<Iterator&>()))) &&                                                           //
      noexcept(make_subrange(std::declval<Iterator&&>(), std::declval<Iterator&&>()))                                 //
  )                                                                                                                   //
      -> bool                                                                                                         //
  {
    // This implements a 'constexpr' version of 'any_of'
    // NOLINTNEXTLINE(readability-use-anyofallof)
    for (auto const& element : arene::base::make_subrange(std::move(begin), std::move(end))) {
      if (pred(proj(element))) {
        return true;
      }
    }
    return false;
  }

  /// @brief Check if any of the elements in a range are true
  /// @tparam Iterator the type of the iterator
  /// @param begin The start of the range
  /// @param end The end of the range
  /// @return bool @c true if any of the elements in the range are @c true, @c false
  /// otherwise.
  ///
  /// This overload casts the elements to @c bool.
  ///
  template <
      typename Iterator,
      constraints<
          std::enable_if_t<base::is_input_iterator_v<Iterator>>,
          std::enable_if_t<std::is_convertible<algorithm_detail::iter_reference_t<Iterator>, bool>::value>> = nullptr>
  constexpr auto operator()(Iterator begin, Iterator end) const noexcept(  //
      is_nothrow_invocable_v<                                              //
          any_of_fn const&,                                                //
          Iterator&&,                                                      //
          Iterator&&,                                                      //
          algorithm_detail::convert_to<bool>>                              //
  )                                                                        //
      -> bool {                                                            // CODEQLFP(DCL51-CPP)
    return (*this)(                                                        //
        std::move(begin),
        std::move(end),
        algorithm_detail::convert_to<bool>{}
    );
  }

  /// @brief Check if any the values in the supplied list are true.
  /// @param values A list of values
  /// @return bool @c true if any of the values are @c true, @c false othewise
  constexpr auto operator()(std::initializer_list<bool> const values) const noexcept -> bool {  // CODEQLFP(DCL51-CPP)
    return (*this)(values.begin(), values.end());
  }

  /// @brief Check if any of the supplied arguments are true.
  /// @tparam Args The types of the arguments
  /// @param args the arguments
  /// @pre All the arguments must be bool.
  /// @return bool @c true if any of the elements are @c true, @c false otherwise
  template <typename... Args, constraints<std::enable_if_t<base::all_are_same_v<bool, Args...>>> = nullptr>
  constexpr auto operator()(Args... args) const noexcept -> bool {  // CODEQLFP(DCL51-CPP) CODEQLFP(A7-1-1)
    return (*this)({args...});                                      // CODEQLFP(M8-5-2)
  }
};
}  // namespace any_of_detail

/// @def arene::base::any_of
/// @brief checks if a predicate is true for any of the elements in a range
/// @copydoc arene::base::any_of_detail::any_of_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(any_of_detail::any_of_fn, any_of);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_ANY_OF_HPP_
