// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::algorithm_detail::heap_sort"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_HEAP_SORT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_HEAP_SORT_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/algorithm/.*"

#include "arene/base/algorithm/detail/make_heap.hpp"
#include "arene/base/algorithm/detail/sort_heap.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/iterator_traits.hpp"

namespace arene {
namespace base {
namespace algorithm_detail {
namespace heap_sort_detail {

/// @brief function object implementing the heap sort algorithm
///
class heap_sort_impl_fn {
 public:
  /// @brief sorts a range into ascending order
  /// @tparam I iterator type
  /// @tparam C compare type
  /// @param first beginning of the range
  /// @param last end of the range
  /// @param comp comparison function object
  ///
  /// @pre @c I must satisfy the random access iterator requirements.
  /// @pre @c I must be ValueSwappable.
  /// @pre <c>[first, last)</c> must be a valid range.
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
  /// Sorts the elements in the range <c> [first, last) </c> in non-descending
  /// order, specified by @c comp. The order of equal elements is not guaranteed
  /// to be preserved.
  ///
  /// @post <c> is_sorted(first, last, comp) </c> is @c true
  ///
  /// @note Complexity <br>
  ///   * Given N as <c> last - first </c>, worst case of <c> O(N * log(N)) </c>
  ///     applications of @c comp.
  ///   * Given N as <c> last - first </c>, worst case of <c> O(N * log(N)) </c>
  ///     move operations.
  ///
  /// @note This sorting algorithm is not stable.
  ///
  template <
      class I,
      class C,
      class = std::enable_if_t<
          is_random_access_iterator_v<I> &&       //
          is_swappable_v<iter_reference_t<I>> &&  //
          is_compare_v<C&, iter_reference_t<I>>>>
  constexpr auto operator()(I first, I last, C comp) const noexcept(  //
      is_nothrow_invocable_v<decltype(make_heap), I&, I&, C&> &&      //
      is_nothrow_invocable_v<decltype(sort_heap), I&, I&, C&>         //
  ) -> void {
    // NOTE: The 'arene::base' versions of these algorithms are function
    // objects. The 'std' versions will not be found
    // by ADL even though the call is unqualified
    make_heap(first, last, comp);
    sort_heap(first, last, comp);
  }
};

}  // namespace heap_sort_detail

/// @def arene::base::algorithm_detail::heap_sort
/// @copydoc arene::base::algorithm_detail::heap_sort_detail::heap_sort_impl_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(heap_sort_detail::heap_sort_impl_fn, heap_sort);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace algorithm_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_DETAIL_HEAP_SORT_HPP_
