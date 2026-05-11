// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_NEXT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_NEXT_HPP_

// IWYU pragma: private, include "arene/base/iterator.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
///
/// @brief Produces the Nth successor (or predecessor if @c steps is negative) of an iterator.
///
/// @tparam Iterator The iterator to advance. Does not participate in overload resolution unless @c Iterator is at
///         least an input iterator.
/// @param itr The iterator to advance
/// @param steps The number of steps to advance the iterator by
/// @pre If incrementing or decrementing @c itr @c steps times results in attempting to produce an invalid iterator
///      such as past-the-end or before-the-begin, behavior is undefined.
/// @pre If @c Iterator is an input iterator, @c steps is not negative, else @c ARENE_PRECONDITION violation.
/// @return The iterator produced by incrementing/decrementing @c itr @c steps times.
///
template <typename Iterator, constraints<std::enable_if_t<is_input_iterator_v<Iterator>>> = nullptr>
ARENE_NODISCARD constexpr auto next(  // CODEQLFP(DCL51-CPP)
    Iterator itr,
    typename std::iterator_traits<Iterator>::difference_type steps = 1
) noexcept(noexcept(::arene::base::advance(itr, steps))) -> Iterator {  // CODEQLFP(EXP52-CPP)
  ::arene::base::advance(itr, steps);
  return itr;
}
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ITERATOR_NEXT_HPP_
