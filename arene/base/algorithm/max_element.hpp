// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::max_element"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_MAX_ELEMENT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_MAX_ELEMENT_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/algorithm/detail/relative_find.hpp"
#include "arene/base/algorithm/detail/traits.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/functional/detail/flip.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/iterator_traits.hpp"
#include "arene/base/functional/identity.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/stdlib_choice/move.hpp"
#include "arene/base/type_traits/is_compare.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"

namespace arene {
namespace base {
namespace algorithm_detail {
namespace max_element_detail {

/// @brief function object implementing the max_element algorithm
class max_element_fn {
 public:
  /// @brief returns the largest element in a range
  /// @tparam ForwardIterator iterator type
  /// @tparam Compare binary predicate type, defaults to <c>std::less<></c>
  /// @tparam Projection projection applied to each element before comparison,
  ///   defaults to <c>arene::base::identity</c>
  /// @param first beginning of the range of elements
  /// @param last end of the range of elements
  /// @param comp binary predicate which returns @c true if the first projected
  ///   argument is ordered before the second projected argument
  /// @param proj projection applied to each element before it is passed to @c comp
  ///
  /// @pre @c ForwardIterator must satisfy the forward iterator requirements.
  /// @pre <c>[begin, end)</c> must be a valid range.
  /// @pre The expression <c>comp(invoke(proj, e1), invoke(proj, e2))</c> must
  ///   be convertible to @c bool for every argument @c e1, @c e2 of type @c RT,
  ///   where @c RT is the reference type of @c ForwardIterator, regardless of
  ///   value category, and must not modify @c e1 or @c e2.
  ///
  /// Finds the largest element, i.e. the element whose projected value is
  /// ordered after the projected values of all other elements, in the range
  /// <c>[first, last)</c>.
  ///
  /// @return iterator to the largest element of the range <c>[first, last)</c>,
  /// i.e. the element @c e for which <c>invoke(proj, e)</c> is ordered after
  /// the projection of every other element. If several elements in the range
  /// are equivalent to the largest element, returns the iterator to the first
  /// such element. Returns @c last if the range is empty.
  ///
  /// @note Complexity
  ///   Given N as <c>std::distance(first, last)</c>, exactly <c>max(N - 1,
  ///   0)</c> applications of the comparator @c comp.
  template <
      class ForwardIterator,
      class Compare = std::less<>,
      class Projection = identity,
      arene::base::constraints<
          std::enable_if_t<is_forward_iterator_v<ForwardIterator>>,
          std::enable_if_t<is_invocable_v<Projection&, iter_reference_t<ForwardIterator>>>,
          std::enable_if_t<is_compare_v<Compare&, invoke_result_t<Projection&, iter_reference_t<ForwardIterator>>>>> =
          nullptr>
  constexpr auto operator()(ForwardIterator first, ForwardIterator last, Compare comp = {}, Projection proj = {}) const
      noexcept(  //
          noexcept(relative_find(std::move(first), std::move(last), functional_detail::flip(comp), proj))
      ) -> ForwardIterator {
    return relative_find(std::move(first), std::move(last), functional_detail::flip(comp), proj);
  }
};

}  // namespace max_element_detail
}  // namespace algorithm_detail

/// @brief Find an iterator to the greatest element of a range.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(algorithm_detail::max_element_detail::max_element_fn, max_element);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_MAX_ELEMENT_HPP_
