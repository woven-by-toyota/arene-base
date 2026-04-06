// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::algorithm_detail::push_heap"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_PUSH_HEAP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_PUSH_HEAP_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/algorithm/.*"

#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/algorithm/iter_swap.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/arithmetic_traits.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar identifiers permitted by M2-10-1 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace algorithm_detail {
namespace push_heap_detail {

/// @brief function object implementing the push_heap algorithm
///
class push_heap_impl_fn {
  // parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: There is no template constructor"

  /// @brief determines the parent for an element in a heap
  /// @tparam I iterator type
  ///
  template <class I>
  class parent_starting_from {
    /// @brief first element of the heap
    I const& first_;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

   public:
    /// @brief specify the first element in the heap
    /// @param first reference to the first element
    ///
    constexpr explicit parent_starting_from(I const& first) noexcept
        : first_{first} {}

    /// @brief determine the parent for an element
    /// @param iter iterator to the child element
    /// @return iterator to the parent element
    ///
    constexpr auto operator()(I const& iter) const noexcept(is_nothrow_addable_v<I, iter_difference_t<I>>) -> I {
      using difference_type = iter_difference_t<I>;
      constexpr difference_type one{1};  // AUTOSAR rule A5-1-1 wants us to do this
      constexpr difference_type two{2};  // AUTOSAR rule A5-1-1 wants us to do this

      auto const index = iter - first_;
      return first_ + ((index - one) / two);
    }
  };

  // parasoft-end-suppress AUTOSAR-A14_5_1-a

 public:
  /// @brief add an element to a max heap
  /// @tparam I iterator type
  /// @tparam C compare type
  /// @param first beginning of the range
  /// @param last end of the range
  /// @param comp comparison function object
  ///
  /// @pre @c I must satisfy the random access iterator requirements.
  /// @pre @c I must be ValueSwappable.
  /// @pre <c>[first, last)</c> must be a valid range.
  /// @pre <c>[first, last - 1)</c> must be a max heap with respect to @c comp.
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
  /// Inserts the element at position <c> last - 1 </c> into the heap <c>
  /// [first, last - 1) </c>. After the insertion, <c> [first, last) </c> is a
  /// heap.
  ///
  /// @post <c> is_heap(first, last, comp) </c> is @c true
  ///
  template <
      class I,
      class C,
      constraints<
          std::enable_if_t<is_random_access_iterator_v<I>>,
          std::enable_if_t<is_swappable_v<iter_reference_t<I>>>,
          std::enable_if_t<is_compare_v<C&, iter_reference_t<I>>>> = nullptr>
  constexpr auto operator()(I first, I last, C comp) const noexcept(           //
      is_nothrow_equality_comparable_v<I>&&                                    //
      noexcept(--first) &&                                                     //
      is_nothrow_invocable_v<parent_starting_from<I> const&, I&> &&            //
      is_nothrow_invocable_v<C&, iter_reference_t<I>, iter_reference_t<I>> &&  //
      is_nothrow_invocable_v<decltype(iter_swap), I&, I&>                      //
  ) -> void {
    if (first == last) {
      return;
    }

    auto const find_parent_of = parent_starting_from<I>{first};
    // parasoft-begin-suppress AUTOSAR-M5_0_15-a "These are iterator types, so decrementing is ok"
    --last;
    // parasoft-end-suppress AUTOSAR-M5_0_15-a
    auto& child = last;

    auto parent = find_parent_of(child);
    while (comp(*parent, *child)) {
      iter_swap(parent, child);
      child = std::move(parent);
      if (child == first) {
        return;
      }
      parent = find_parent_of(child);
    }
  }
};

}  // namespace push_heap_detail

/// @def arene::base::algorithm_detail::push_heap
/// @copydoc arene::base::push_heap_detail::push_heap_impl_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(algorithm_detail::push_heap_detail::push_heap_impl_fn, push_heap);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_PUSH_HEAP_HPP_
