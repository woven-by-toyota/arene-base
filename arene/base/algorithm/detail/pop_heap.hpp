// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::algorithm_detail::pop_heap"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_POP_HEAP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_POP_HEAP_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/algorithm/.*"

#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/algorithm/iter_swap.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/iterator/distance.hpp"
#include "arene/base/iterator/next.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/initializer_list.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_move_assignable.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/arithmetic_traits.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace algorithm_detail {
namespace pop_heap_detail {

/// @brief function object implementing the pop_heap algorithm
///
class pop_heap_impl_fn {
  /// @brief determines the iterator referring to the largest among a parent and its child elements in a
  ///     heap
  /// @tparam I iterator type
  /// @tparam C compare type
  ///
  template <class I, class C>
  class largest_starting_from {
    /// @brief first element in the heap
    I first_;
    /// @brief number of elements in the heap
    iter_difference_t<I> size_{};
    /// @brief comparator for elements in the heap
    C& comp_;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

   public:
    /// @brief specify fixed parameters for determining the largest element
    /// @param first first element in the heap
    /// @param size number of elements in the heap
    /// @param comp comparator for elements in the heap
    ///
    constexpr largest_starting_from(I const& first, iter_difference_t<I> size, C& comp) noexcept(
        std::is_nothrow_copy_constructible<I>::value
    )
        : first_{first},
          size_{size},
          comp_{comp} {}

    /// @brief determine if the child elements are larger than the parent
    /// @param iter iterator to the parent element
    ///
    /// Returns an iterator to the largest of the following:
    /// * @c iter (the parent element)
    /// * first child of @c iter (if present)
    /// * second child of @c iter (if present)
    ///
    /// @c size_ is used to determine if @c iter has children.
    ///
    /// @return iterator to the largest element
    ///
    constexpr auto operator()(I const& iter) const noexcept(  //
           is_nothrow_subtractable_v<I> && //
           is_nothrow_addable_v<I, iter_difference_t<I>> && //
           is_nothrow_invocable_v<C&, iter_reference_t<I>, iter_reference_t<I>>
    ) -> I {
      using difference_type = iter_difference_t<I>;
      constexpr difference_type one{1};  // AUTOSAR rule A5-1-1 wants us to do this
      constexpr difference_type two{2};  // AUTOSAR rule A5-1-1 wants us to do this

      auto const parent_position = iter - first_;
      auto const child_offset = two * parent_position;
      auto position_of_largest = parent_position;

      for (difference_type const index : {one, two}) {
        auto const position = child_offset + index;
        if (position >= size_) {
          break;
        }

        if (comp_(*arene::base::next(first_, position_of_largest), *arene::base::next(first_, position))) {
          position_of_largest = position;
        }
      }

      return arene::base::next(first_, position_of_largest);
    }
  };

 public:
  /// @brief removes the largest element from a max heap
  /// @tparam I iterator type
  /// @tparam C compare type
  /// @param first beginning of the range
  /// @param last end of the range
  /// @param comp comparison function object
  ///
  /// @pre @c I must satisfy the random access iterator requirements.
  /// @pre @c I must be ValueSwappable.
  /// @pre <c>[first, last)</c> must be a valid range.
  /// @pre <c>[first, last)</c> is not an empty range.
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
  /// Removes the first element in heap <c> [first, last) </c> by swapping the
  /// value in the position @c first and the value in position <c> last - 1
  /// </c>. Then makes the subrange <c> [first, last - 1) </c> into a heap.
  ///
  /// @post <c> is_heap(first, last - 1 , comp) </c> is @c true
  ///
  template <
      class I,
      class C,
      constraints<
          std::enable_if_t<is_random_access_iterator_v<I>>,
          std::enable_if_t<is_swappable_v<iter_reference_t<I>>>,
          std::enable_if_t<is_compare_v<C&, iter_reference_t<I>>>> = nullptr>
  constexpr auto operator()(I first, I last, C comp) const noexcept(     //
      noexcept(--first) &&                                               //
      is_nothrow_subtractable_v<I> &&                                    //
      is_nothrow_invocable_v<decltype(iter_swap), I&, I&> &&             //
      std::is_nothrow_move_assignable<I>::value &&                       //
      is_nothrow_invocable_v<largest_starting_from<I, C> const&, I&> &&  //
      std::is_nothrow_copy_constructible<I>::value                       //
  ) -> void {
    if (arene::base::distance(first, last) <= iter_difference_t<I>{1}) {
      return;
    }

    // parasoft-begin-suppress AUTOSAR-M5_0_15-a "These are iterator types, so decrementing is ok"
    --last;
    // parasoft-end-suppress AUTOSAR-M5_0_15-a
    iter_swap(first, last);

    auto const find_largest_child_of = largest_starting_from<I, C>{first, arene::base::distance(first, last), comp};
    auto& parent = first;

    while (true) {
      auto child = find_largest_child_of(parent);
      if (parent == child) {
        return;
      }

      iter_swap(parent, child);
      parent = std::move(child);
    }
  }
};

}  // namespace pop_heap_detail

/// @def arene::base::algorithm_detail::pop_heap
/// @copydoc arene::base::pop_heap_detail::pop_heap_impl_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(algorithm_detail::pop_heap_detail::pop_heap_impl_fn, pop_heap);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_POP_HEAP_HPP_
