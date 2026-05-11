// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::inplace_merge"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_INPLACE_MERGE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_INPLACE_MERGE_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/algorithm/detail/bind_back_with_iterator.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/algorithm/find_if.hpp"
#include "arene/base/algorithm/rotate.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/functional/not_fn.hpp"
#include "arene/base/iterator/advance.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/denotes_range.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_movable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

// IWYU pragma: no_include "arene/base/stdlib_choice/iterator_traits.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_cv.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_reference.hpp"

namespace arene {
namespace base {
namespace algorithm_detail {
namespace inplace_merge_detail {

/// @brief function object implementing the @c inplace_merge algorithm
///
class inplace_merge_impl_fn {
 public:
  /// @brief merges a range consisting of two ordered sub-ranges in-place so the whole range is sorted
  /// @tparam BidirectionalIt iterator type
  /// @tparam Compare compare type
  /// @param first beginning of the first range
  /// @param middle end of the first range, beginning of the second range
  /// @param last end of the second range
  /// @param comp comparison function object
  ///
  /// @pre @c BidirectionalIt must satisfy the bidirectional iterator requirements.
  /// @pre @c BidiectionalIt must be ValueSwappable.
  /// @pre the type of <c>*first</c> must be Movable.
  /// @pre @c Compare must be Compare.
  /// @pre <c>[first, last)</c> must be a valid range.
  /// @pre <c> is_sorted(first, middle, comp) </c> is @c true
  /// @pre <c> is_sorted(middle, last, comp) </c> is @c true
  /// @pre The expression <c>comp(r1, r2)</c> must be convertible to @c bool
  ///   for every argument @c r1 and @c r2 of type @c RT, where @c RT is the
  ///   reference type of @c I, regardless of value category, and must not modify
  ///   @c r1 or @c r2.
  /// @pre comp must define a *strict weak ordering relation*. For values
  ///   @c a, @c b, and @c c, the following properties must be satisfied:
  ///   * irreflexivity: <c> comp(a, a) </c> is @c false
  ///   * transitivity: if <c> comp(a, b) </c> and <c> comp(b, c) </c> are both
  ///     @c true, then <c> comp(a, c) </c> is @c true
  ///   * transitivity of incomparability: let <c> e(a, b) </c> be
  ///     <c> !comp(a, b) && !comp(b, a) </c>, then @c e is transitive
  ///
  /// Merges two consecutive sorted ranges <c> [first, middle) </c> and <c>
  /// [middle, last) </c> into one sorted range <c> [first, last) </c>.
  ///
  /// This merge function is stable, which means that for equivalent elements in
  /// the original two ranges, the elements from the first range (preserving
  /// their original order) precede the elements from the second range
  /// (preserving their original order).
  ///
  /// @post <c> is_sorted(first, last, comp) </c> is @c true
  ///
  /// @note Complexity
  ///   Given N as <c> last - first </c>, worst case of <c> O(N * log(N)) </c>
  ///   applications of @c comp.
  /// @note This implementation never tries to allocate additional memory.
  template <
      class BidirectionalIt,
      class Compare,
      constraints<
          std::enable_if_t<is_bidirectional_iterator_v<BidirectionalIt>>,
          std::enable_if_t<is_swappable_v<iter_reference_t<BidirectionalIt>>>,
          std::enable_if_t<is_movable_v<remove_cvref_t<iter_reference_t<BidirectionalIt>>>>,
          std::enable_if_t<is_compare_v<Compare&, iter_reference_t<BidirectionalIt>>>> = nullptr>
  constexpr auto operator()(BidirectionalIt first, BidirectionalIt middle, BidirectionalIt last, Compare comp) const
      noexcept(                                                                                          //
          is_nothrow_equality_comparable_v<BidirectionalIt> &&                                           //
          denotes_nothrow_iterable_range_v<BidirectionalIt> &&                                           //
          std::is_nothrow_copy_constructible<BidirectionalIt>::value&&                                   //
          noexcept(arene::base::find_if(                                                                 //
              std::declval<BidirectionalIt>(),                                                           //
              std::declval<BidirectionalIt&>(),                                                          //
              std::declval<bind_back_with_iterator<Compare, BidirectionalIt>>()                          //
          )) &&                                                                                          //
          is_nothrow_invocable_v<decltype(rotate), BidirectionalIt, BidirectionalIt, BidirectionalIt>&&  //
          noexcept(arene::base::advance(                                                                 //
              std::declval<BidirectionalIt&>(),                                                          //
              arene::base::distance(                                                                     //
                  std::declval<BidirectionalIt&>(),                                                      //
                  std::declval<BidirectionalIt&>()                                                       //
              )                                                                                          //
          )) &&                                                                                          //
          std::is_nothrow_move_assignable<BidirectionalIt>::value                                        //
      ) -> void {
    auto left = first;
    auto right = middle;

    while ((left != middle) && (right != last)) {
      auto right_end = arene::base::find_if(  //
          right,
          last,
          not_fn(bind_back_with_iterator<Compare, BidirectionalIt>{comp, left})
      );
      if (right_end == right) {
        ++left;
      } else {
        auto const block_size = arene::base::distance(right, right_end);
        left = rotate(left, right, right_end);
        arene::base::advance(middle, block_size);
        right = std::move(right_end);
      }
    }
  }
};

}  // namespace inplace_merge_detail
}  // namespace algorithm_detail

/// @def arene::base::inplace_merge
/// @copydoc arene::base::algorithm_detail::inplace_merge_detail::inplace_merge_impl_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(algorithm_detail::inplace_merge_detail::inplace_merge_impl_fn, inplace_merge);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_INPLACE_MERGE_HPP_
