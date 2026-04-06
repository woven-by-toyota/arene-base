// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::iter_swap"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_ITER_SWAP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_ITER_SWAP_HPP_

// IWYU pragma: private, include "arene/base/algorithm.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/type_traits/iterator_category_traits.hpp"
#include "arene/base/utility/swap.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace algorithm_detail {
namespace iter_swap_detail {
/// @brief function object that implements @c iter_swap
class iter_swap_impl_fn {
 public:
  /// @brief Swaps the values of the elements the given iterators are pointing to.
  /// @tparam IteratorA The type of the Iterator for the first value to swap
  /// @tparam IteratorB The type of the Iterator for the second value to swap
  /// @param lhs Iterator to first value to swap
  /// @param rhs Iterator to second value to swap
  template <
      class IteratorA,
      class IteratorB,
      constraints<
          std::enable_if_t<is_forward_iterator_v<IteratorA>>,
          std::enable_if_t<is_forward_iterator_v<IteratorB>>> = nullptr>
  constexpr auto operator()(IteratorA lhs, IteratorB rhs) const noexcept(noexcept(::arene::base::swap(*lhs, *rhs)))
      -> void {
    ::arene::base::swap(*lhs, *rhs);
  }
};
}  // namespace iter_swap_detail
}  // namespace algorithm_detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(algorithm_detail::iter_swap_detail::iter_swap_impl_fn, iter_swap);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ALGORITHM_ITER_SWAP_HPP_
