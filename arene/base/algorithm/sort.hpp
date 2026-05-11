// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::sort"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_SORT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_SORT_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/algorithm/detail/heap_sort.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_movable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/iterator_traits.hpp"

namespace arene {
namespace base {
namespace algorithm_detail {
namespace sort_detail {

/// @brief function object implementing the sort algorithm
///
class sort_impl_fn {
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
  /// @pre the type of <c>*first</c> must be Movable.
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
  /// @note Complexity
  ///   Given N as <c> last - first </c>, worst case of <c> O(N * log(N)) </c>
  ///   applications of @c comp.
  ///
  template <
      class I,
      class C,
      class = std::enable_if_t<
        is_random_access_iterator_v<I> &&
        is_swappable_v<iter_reference_t<I>> &&
        is_movable_v<remove_cvref_t<iter_reference_t<I>>> &&
        is_compare_v<C&, iter_reference_t<I>>
      >>
  constexpr auto operator()(I first, I last, C comp) const noexcept( //
      is_nothrow_invocable_v<decltype(heap_sort), I&&, I&&, C&&>
  ) -> void {
    heap_sort(std::move(first), std::move(last), std::move(comp));
  }

  /// @brief sorts a range into ascending order
  /// @tparam I iterator type
  /// @param first beginning of the range
  /// @param last end of the range
  ///
  /// @pre @c I must satisfy the random access iterator requirements.
  /// @pre @c I must be ValueSwappable.
  /// @pre the type of <c>*first</c> must be Movable.
  /// @pre <c>[first, last)</c> must be a valid range.
  /// @pre The expression <c>std::less<>{}(r1, r2)</c> must be convertible to @c bool
  ///   for every argument @c r1 and @c r2 of type @c RT, where @c RT is the
  ///   reference type of @c I, regardless of value category, and must not modify
  ///   @c r1 or @c r2.
  /// @pre @c std::less<> must define a *strict weak ordering relation*. For values
  ///   @c a, @c b, and @c c, the following properties must be satisfied:
  ///   * irreflexivity: <c> std::less<>{}(a, a) </c> is @c false
  ///   * transitivity: if <c> std::less<>{}(a, b) </c> and
  ///     <c> std::less<>{}(b, c) </c> are both @c true, then
  ///     <c> std::less<>{}(a, c) </c> is @c true
  ///   * transitivity of incomparability: let <c> e(a, b) </c> be
  ///     <c> !std::less<>{}(a, b) && !std::less<>{}(b, a) </c>, then @c e is
  ///     transitive
  ///
  /// Sorts the elements in the range <c> [first, last) </c> in non-descending
  /// order, specified by @c std::less<>. The order of equal elements is not
  /// guaranteed to be preserved.
  ///
  /// @note Complexity
  ///   Given N as <c> last - first </c>, worst case of <c> O(N * log(N)) </c>
  ///   applications of @c std::less<>.
  ///
  template <
      class I,
      class = std::enable_if_t<
          is_random_access_iterator_v<I> &&
          is_swappable_v<iter_reference_t<I>> &&
          is_movable_v<remove_cvref_t<iter_reference_t<I>>> &&
          is_compare_v<std::less<>&, iter_reference_t<I>>>>
  constexpr auto operator()(I first, I last) const noexcept( //
      is_nothrow_invocable_v<sort_impl_fn const&, I&&, I&&, std::less<>>
  ) -> void {
    return (*this)(std::move(first), std::move(last), std::less<>{});
  }
};

}  // namespace sort_detail
}  // namespace algorithm_detail

/// @def arene::base::sort
/// @copydoc arene::base::algorithm_detail::sort_detail::sort_impl_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(algorithm_detail::sort_detail::sort_impl_fn, sort);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_SORT_HPP_
