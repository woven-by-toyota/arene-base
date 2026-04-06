// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTERNAL_RELATIVE_FIND_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTERNAL_RELATIVE_FIND_HPP_

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/type_traits/is_predicate.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_copy_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_copy_constructible.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: friend "stdlib_detail/.*"

namespace std {
namespace internal {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

/// @brief finds the element that best satisfies a relative metric
/// @tparam ForwardIterator iterator type
/// @tparam BinaryPredicate binary predicate type
/// @param first beginning of the range of elements
/// @param last end of the range of elements
/// @param pred binary predicate type implementing the relative metric
///
/// @return iterator to the first element that best satisfies @c pred.
///
template <
    class ForwardIterator,
    class BinaryPredicate,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator>>,
        std::enable_if_t<arene::base::is_predicate_v<
            BinaryPredicate&,
            arene::base::algorithm_detail::iter_reference_t<ForwardIterator>,
            arene::base::algorithm_detail::iter_reference_t<ForwardIterator>>>> = nullptr>
constexpr auto relative_find(ForwardIterator first, ForwardIterator last, BinaryPredicate pred) noexcept(
    std::is_nothrow_copy_constructible_v<ForwardIterator> &&  //
    std::is_nothrow_copy_assignable_v<ForwardIterator>&&      //
    noexcept(first == last) &&                                //
    noexcept(++first != last) &&                              //
    noexcept(pred(*first, *first))
) -> ForwardIterator {
  if (first == last) {
    return first;
  }

  auto best = first;

  // parasoft-begin-suppress AUTOSAR-M5_0_15-a-2 "This is an iterator type, so incrementing is OK"
  ++first;
  // parasoft-end-suppress AUTOSAR-M5_0_15-a-2
  while (first != last) {
    if (pred(*first, *best)) {
      best = first;
    }
    // parasoft-begin-suppress AUTOSAR-M5_0_15-a-2 "This is an iterator type, so incrementing is OK"
    ++first;
    // parasoft-end-suppress AUTOSAR-M5_0_15-a-2
  }

  return best;
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace internal
}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTERNAL_RELATIVE_FIND_HPP_
