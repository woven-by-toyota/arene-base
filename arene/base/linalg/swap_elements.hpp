// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::swap_elements"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_SWAP_ELEMENTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_SWAP_ELEMENTS_HPP_

#include "arene/base/array/array.hpp"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/index_permutation_range.hpp"
#include "arene/base/linalg/detail/mandates.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_swappable.hpp"
#include "arene/base/utility/swap.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a-2 "Similar names permitted by M2-10-1 Permit #1"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace linalg {

namespace swap_elements_detail {

/// @brief Class to implement @c swap_elements
class swap_elements_fn {
 public:
  /// @brief Swap the elements of two <c>mdspan</c>s with each other
  /// @tparam LeftSpan The type of the left @c mdspan, must be the same rank as @c RightSpan
  /// @tparam RightSpan The type of the right @c mdspan, must be the same rank as @c LeftSpan
  /// @param left The first @c mdspan to swap elements from
  /// @param right The second @c mdspan to swap elements from
  /// @note This corresponds to the SSWAP and DSWAP functions from BLAS 1 (xSWAP).
  /// @pre <c>left.extents() == right.extents()</c>, otherwise precondition violation
  /// @pre if @c left and @c right overlap, they must alias
  template <
      typename LeftSpan,
      typename RightSpan,
      arene::base::constraints<
        linalg_detail::possibly_comparable_constraints<LeftSpan, RightSpan>,
          std::enable_if_t<
              is_swappable_with_v<typename LeftSpan::reference, typename RightSpan::reference>>> = nullptr>
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: All parameters are used"
  constexpr auto operator()(LeftSpan const left, RightSpan const right) const noexcept(  //
      is_nothrow_swappable_with_v<typename RightSpan::reference, typename LeftSpan::reference> &&
      is_nothrow_invocable_v<LeftSpan const&, array<typename LeftSpan::index_type, LeftSpan::rank()>> &&
      is_nothrow_invocable_v<RightSpan const&, array<typename LeftSpan::index_type, RightSpan::rank()>>
  ) -> void {
    // parasoft-end-suppress AUTOSAR-A0_1_4-a
    ARENE_PRECONDITION(left.extents() == right.extents());
    if (left.empty()) {
      return;
    }
    auto const extents_array = mdspan_detail::to_array(left.extents());
    // parasoft-begin-suppress AUTOSAR-M0_1_3 "False positive: All local variables are used"
    for (auto const& indices : arene::base::detail::make_index_permutation_range(extents_array)) {
      arene::base::swap(left(indices), right(indices));
    }
    // parasoft-end-suppress AUTOSAR-M0_1_3
  }
};
}  // namespace swap_elements_detail

/// @brief Swap corresponding elements of two multidimensional views.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(swap_elements_detail::swap_elements_fn, swap_elements);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_SWAP_ELEMENTS_HPP_
