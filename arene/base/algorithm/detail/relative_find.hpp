// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::detail::relative_find"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_RELATIVE_FIND_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_RELATIVE_FIND_HPP_

#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_copy_assignable.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/functional/invoke.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"

namespace arene {
namespace base {
namespace algorithm_detail {
namespace relative_find_detail {

/// @brief function object implementing the relative_find algorithm
class relative_find_fn {
 public:
  /// @brief finds the element that best satisfies a relative metric
  /// @tparam ForwardIterator iterator type
  /// @tparam Compare binary predicate type
  /// @tparam Projection projection applied to each element before comparison
  /// @param first beginning of the range of elements
  /// @param last end of the range of elements
  /// @param comp binary predicate type implementing the relative metric
  /// @param proj projection applied to each element before it is passed to @c comp
  /// @return iterator to the first element whose projection best satisfies @c comp
  /// @attention This algorithm *only* checks if @c ForwardIterator satisfies
  /// input-iterator to allow use with iterators that satisfy
  /// @c ranges::forward_iterator (i.e. the iterator provides a multi-pass
  /// guarantee but <c>operator*()</c> does *not* return a real reference).
  template <
      class ForwardIterator,
      class Compare,
      class Projection,
      arene::base::constraints<
          std::enable_if_t<is_input_iterator_v<ForwardIterator>>,
          std::enable_if_t<is_invocable_v<Projection&, iter_reference_t<ForwardIterator>>>,
          std::enable_if_t<is_compare_v<Compare&, invoke_result_t<Projection&, iter_reference_t<ForwardIterator>>>>> =
          nullptr>
  constexpr auto operator()(ForwardIterator first, ForwardIterator last, Compare comp, Projection& proj) const noexcept(
      std::is_nothrow_copy_constructible<ForwardIterator>::value &&  //
      std::is_nothrow_copy_assignable<ForwardIterator>::value &&     //
      is_nothrow_equality_comparable_v<ForwardIterator> &&           //
      is_nothrow_inequality_comparable_v<ForwardIterator>&&          //
      noexcept(++first) &&                                           //
      noexcept(comp(arene::base::invoke(proj, *first), arene::base::invoke(proj, *first)))
  ) -> ForwardIterator {
    if (first == last) {
      return first;
    }

    auto best = first;

    // parasoft-begin-suppress AUTOSAR-M5_0_15-a-2 "This is an iterator type, so incrementing is OK"
    ++first;
    // parasoft-end-suppress AUTOSAR-M5_0_15-a-2
    while (first != last) {
      if (comp(arene::base::invoke(proj, *first), arene::base::invoke(proj, *best))) {
        best = first;
      }
      // parasoft-begin-suppress AUTOSAR-M5_0_15-a-2 "This is an iterator type, so incrementing is OK"
      ++first;
      // parasoft-end-suppress AUTOSAR-M5_0_15-a-2
    }

    return best;
  }
};

}  // namespace relative_find_detail

/// @def arene::base::algorithm_detail::relative_find
/// @copydoc arene::base::relative_find_detail::relative_find_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(algorithm_detail::relative_find_detail::relative_find_fn, relative_find);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_RELATIVE_FIND_HPP_
