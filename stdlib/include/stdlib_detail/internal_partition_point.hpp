// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTERNAL_PARTITION_POINT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTERNAL_PARTITION_POINT_HPP_

#include "arene/base/algorithm/detail/functional.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/constraints.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/type_traits/is_predicate.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_move_assignable.hpp"
#include "stdlib/include/stdlib_detail/is_move_constructible.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: friend "stdlib_detail/.*"

namespace std {
namespace internal {

// parasoft-begin-suppress AUTOSAR-M3_3_2-a "False positive: inline function used in multiple translation units"

/// @brief locates the partition point of a range partitioned with respect to an unary predicate
/// @tparam ForwardIterator iterator type
/// @tparam UnaryPred unary predicate type
/// @param first beginning of the partitioned range of elements
/// @param last end of the partitioned range of elements
/// @param pred predicate specifying the partition
///
/// @return iterator past the end of the first partition within <tt>[first,
///     last)</tt> or @c last if all elements satisfy @c pred.
///
template <
    class ForwardIterator,
    class UnaryPred,
    arene::base::constraints<
        std::enable_if_t<arene::base::is_forward_iterator_v<ForwardIterator>>,
        std::enable_if_t<arene::base::is_predicate_v<
            UnaryPred&,
            arene::base::algorithm_detail::iter_reference_t<ForwardIterator>>>> = nullptr>
constexpr auto partition_point(ForwardIterator first, ForwardIterator last, UnaryPred pred) noexcept(
    std::is_nothrow_move_constructible_v<ForwardIterator> and  //
    std::is_nothrow_move_assignable_v<ForwardIterator>and      //
    noexcept(arene::base::distance(first, last)) and           //
    noexcept(arene::base::next(first)) and                     //
    noexcept(pred(*first))
) -> ForwardIterator {
  using difference_type = arene::base::algorithm_detail::iter_difference_t<ForwardIterator>;
  constexpr difference_type two{2};

  using arene::base::distance;
  using arene::base::next;

  auto length = distance(first, last);
  while (length > difference_type{}) {
    auto const half = length / two;
    auto const middle = next(first, half);

    if (pred(*middle)) {
      first = next(middle);
      length -= (half + difference_type{1});
    } else {
      length = half;
    }
  }

  return first;
}

// parasoft-end-suppress AUTOSAR-M3_3_2-a

}  // namespace internal
}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_INTERNAL_PARTITION_POINT_HPP_
