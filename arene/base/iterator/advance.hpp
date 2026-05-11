// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @file advance.hpp
/// @brief Provides a backport of C++17's constexpr std::advance.
///

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ADVANCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ADVANCE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/iterator_tags.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

namespace advance_detail {

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
///
/// @brief Implementation of advance for input iterators.
///
/// @tparam Iterator The iterator to advance
/// @param itr The iterator to advance
/// @param steps The number of steps to advance the iterator by.
/// @pre steps is not negative, else @c ARENE_PRECONDITION violation.
/// @pre If incrementing @c itr @c steps times results in attempting to produce an invalid iterator such as
///      past-the-end, behavior is undefined.
/// @post The iterator is incremented @c steps times.
///
template <typename Iterator>
constexpr void do_advance(
    Iterator& itr,
    typename std::iterator_traits<Iterator>::difference_type steps,
    std::input_iterator_tag
) noexcept(noexcept(++itr)) {
  ARENE_PRECONDITION(steps >= 0);
  while (steps > 0) {
    ++itr;
    --steps;
  }  // CODEQLFP(A5-0-2)
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2
// parasoft-end-suppress CERT_C-EXP37-a-3

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
/// @brief Implementation of advance for bidirectional iterators.
///
/// @tparam Iterator The iterator to advance
/// @param itr The iterator to advance
/// @param steps The number of steps to advance the iterator by. If positive, the iterator is incremented. If
///            negative, it is decremented.
/// @pre If incrementing or decrementing @c itr @c steps times results in attempting to produce an invalid iterator
///      such as past-the-end or before-the-begin, behavior is undefined.
/// @post The iterator is incremented/decremented @c steps times.
///
template <typename Iterator>
constexpr void do_advance(
    Iterator& itr,
    typename std::iterator_traits<Iterator>::difference_type steps,
    std::bidirectional_iterator_tag
) noexcept(noexcept(++itr) && noexcept(--itr)) {
  while (steps > 0) {
    ++itr;
    --steps;
  }  // CODEQLFP(A5-0-2)
  while (steps < 0) {
    --itr;
    ++steps;
  }  // CODEQLFP(A5-0-2)
}
// parasoft-end-suppress CERT_C-EXP37-a-3

// parasoft-begin-suppress CERT_C-EXP37-a-3 "False positive: The rule does not mention naming all parameters"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
///
/// @brief Implementation of advance for random_access iterators.
///
/// @tparam Iterator The iterator to advance
/// @param itr The iterator to advance
/// @param steps The number of steps to advance the iterator by. If positive, the iterator is incremented. If
///            negative, it is decremented.
/// @pre If incrementing or decrementing @c itr @c steps times results in attempting to produce an invalid iterator
///      such as past-the-end or before-the-begin, behavior is undefined.
/// @post The iterator is incremented/decremented @c steps times.
///
template <typename Iterator>
constexpr void do_advance(
    Iterator& itr,
    typename std::iterator_traits<Iterator>::difference_type steps,
    std::random_access_iterator_tag
) noexcept(noexcept(itr += steps)) {  // CODEQLFP(EXP52-CPP)
  itr += steps;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2
// parasoft-end-suppress CERT_C-EXP37-a-3

/// @brief Implementation helper for @c arene::base::advance
class advance {
 public:
  ///
  /// @brief Increments or decrements an iterator by a set number of steps.
  ///
  /// @tparam Iterator The iterator to advance. Does not participate in overload resolution unless @c Iterator is at
  ///         least an input iterator.
  /// @param itr The iterator to advance
  /// @param steps The number of steps to advance the iterator by. If positive, the iterator is incremented. If
  ///            negative, it is decremented.
  /// @pre If incrementing or decrementing @c itr @c steps times results in attempting to produce an invalid iterator
  ///      such as past-the-end or before-the-begin, behavior is undefined.
  /// @pre If @c Iterator is an input iterator, @c steps is not negative, else @c ARENE_PRECONDITION violation.
  /// @post The iterator is incremented/decremented @c steps times.
  ///
  template <typename Iterator, constraints<std::enable_if_t<is_input_iterator_v<Iterator>>> = nullptr>
  constexpr void operator()(Iterator& itr, typename std::iterator_traits<Iterator>::difference_type steps) const
      noexcept(noexcept(::arene::base::advance_detail::do_advance(
          itr,
          steps,
          typename std::iterator_traits<Iterator>::iterator_category{}
      ))) {  // CODEQLFP(EXP52-CPP)
    ::arene::base::advance_detail::do_advance(itr, steps, typename std::iterator_traits<Iterator>::iterator_category{});
  }
};

}  // namespace advance_detail

/// @def arene::base::advance
/// @copydoc arene::base::advance_detail::advance_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(advance_detail::advance, advance);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_ADVANCE_HPP_
