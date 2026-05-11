// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::algorithm_detail::sort_heap"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_SORT_HEAP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_SORT_HEAP_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/algorithm/.*"

#include "arene/base/algorithm/detail/pop_heap.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"

namespace arene {
namespace base {
namespace algorithm_detail {
namespace sort_heap_detail {

/// @brief function object implementing the sort_heap algorithm
///
class sort_heap_impl_fn {
 public:
  /// @brief turns a max heap into a sorted range of elements
  /// @tparam I iterator type
  /// @tparam C compare type
  /// @param first beginning of the range
  /// @param last end of the range
  /// @param comp comparison function object
  ///
  /// @pre @c I must satisfy the random access iterator requirements.
  /// @pre @c I must be ValueSwappable.
  /// @pre <c>[first, last)</c> must be a valid range.
  /// @pre <c>[first, last)</c> must be a max heap with respect to @c comp.
  /// @pre The expression <c>comp(r1, r2)</c> must be convertible to @c bool
  ///   for every argument @c r1 and @c r2 of type @c RT, where @c RT is the
  ///   reference type of @c I, regardless of value category, and must not modify
  ///   @c r1 or @c r2.
  /// @pre comp must define a *strict weak ordering relation*. For values
  ///   @c a, @c b, and @c c, the following properties must be satisfied:
  ///   * irreflexivity: <c> comp(a, a) </c> is @c false
  ///   * transitivity: if <c> comp(a, b) </c> and <c> comp(b, c) </c> are both
  ///     @c true, then <c> pred(a, c) </c> is @c true
  ///   * transitivity of incomparability: let <c> e(a, b) </c> be
  ///     <c> !comp(a, b) && !comp(b, a) </c>, then @c e is transitive
  ///
  /// Converts the heap <c> [first, last) </c> into a sorted range. The heap
  /// property is no longer maintained.
  ///
  /// @post <c> is_sorted(first, last, comp) </c> is @c true
  ///
  template <
      class I,
      class C,
      constraints<
          std::enable_if_t<is_random_access_iterator_v<I>>,
          std::enable_if_t<is_swappable_v<iter_reference_t<I>>>,
          std::enable_if_t<is_compare_v<C&, iter_reference_t<I>>>> = nullptr>
  constexpr auto operator()(I first, I last, C comp) const noexcept(  //
      noexcept(--last) &&                                             //
      is_nothrow_inequality_comparable_v<I> &&                        //
      is_nothrow_invocable_v<decltype(pop_heap), I&, I&, C&>          //
  ) -> void {
    while (first != last) {
      arene::base::algorithm_detail::pop_heap(first, last, comp);
      // parasoft-begin-suppress AUTOSAR-M5_0_15-a "These are iterator types, so decrementing is ok"
      --last;
      // parasoft-end-suppress AUTOSAR-M5_0_15-a
    }
  }
};

}  // namespace sort_heap_detail

/// @def arene::base::algorithm_detail::short_heap
/// @copydoc arene::base::sort_heap_detail::sort_heap_impl_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(algorithm_detail::sort_heap_detail::sort_heap_impl_fn, sort_heap);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_SORT_HEAP_HPP_
