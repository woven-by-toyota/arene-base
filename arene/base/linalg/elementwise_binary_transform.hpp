// parasoft-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::linalg::elementwise_binary_transform"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_ELEMENTWISE_BINARY_TRANSFORM_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_ELEMENTWISE_BINARY_TRANSFORM_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/index_permutation_range.hpp"
#include "arene/base/linalg/detail/mandates.hpp"
#include "arene/base/mdspan/detail/to_array.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_assignable.hpp"
#include "arene/base/type_traits/is_invocable.hpp"

namespace arene {
namespace base {
namespace linalg {

namespace elementwise_binary_transform_detail {
/// @brief Implementation class for elementwise binary transform
class elementwise_binary_transform_fn {
 public:
  /// @brief Apply a binary transform to the corresponding elements of the two input spans, and assign the result to the
  /// corresponding element of the output span.
  /// @tparam LhsInSpan The type of the first input span
  /// @tparam RhsInSpan The type of the second input span
  /// @tparam OutSpan The type of the output span
  /// @tparam Transform The type of the transform
  /// @param lhs_span The first input span
  /// @param rhs_span The second input span
  /// @param out_span The output span
  /// @param transform The transform operation to apply
  /// @pre All of @c LhsInSpan , @c RhsInSpan , and @c OutSpan must be instantiations of @c mdspan with the same @c rank
  /// @pre if @c lhs_in_span and @c out_span overlap, they must alias
  /// @pre if @c rhs_in_span and @c out_span overlap, they must alias
  /// @pre The specified @c Transform must be invocable with elements of the input spans, and the result must be
  /// assignable to the elements of the output span
  /// @pre All of @c lhs_span , @c rhs_span , and @c out_span must have the same extents
  template <
      typename LhsInSpan,
      typename RhsInSpan,
      typename OutSpan,
      typename Transform,
      constraints<
          linalg_detail::possibly_addable_constraints<LhsInSpan, RhsInSpan, OutSpan>,
          std::enable_if_t<is_invocable_v<Transform&, typename LhsInSpan::reference, typename RhsInSpan::reference>>,
          std::enable_if_t<std::is_assignable<
              typename OutSpan::reference,
              invoke_result_t<Transform&, typename LhsInSpan::reference, typename RhsInSpan::reference>>::value>> =
          nullptr>
  // parasoft-begin-suppress AUTOSAR-A0_1_4-a "False positive: All parameters are used"
  constexpr void
  operator()(LhsInSpan const lhs_span, RhsInSpan const rhs_span, OutSpan const out_span, Transform transform) const
      noexcept(  //
          is_nothrow_invocable_v<Transform&, typename LhsInSpan::reference, typename RhsInSpan::reference> &&
          std::is_nothrow_assignable<
              typename OutSpan::reference,
              invoke_result_t<Transform&, typename LhsInSpan::reference, typename RhsInSpan::reference>>::value
      ) {
    // parasoft-end-suppress AUTOSAR-A0_1_4-a
    ARENE_PRECONDITION(lhs_span.extents() == rhs_span.extents());
    ARENE_PRECONDITION(lhs_span.extents() == out_span.extents());
    if (lhs_span.empty()) {
      return;
    }
    auto const extents_array = mdspan_detail::to_array(lhs_span.extents());
    // parasoft-begin-suppress AUTOSAR-M0_1_3 "False positive: All local variables are used"
    for (auto const& indices : arene::base::detail::make_index_permutation_range(extents_array)) {
      // parasoft-end-suppress AUTOSAR-M0_1_3
      out_span(indices) = transform(lhs_span(indices), rhs_span(indices));
    }
  }
};
}  // namespace elementwise_binary_transform_detail

/// @brief Apply a binary operation to corresponding elements of two multidimensional views.
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
// parasoft-begin-suppress CERT_CPP-DCL59-a "An unnamed namespace is used to create a per-TU reference to a global
// object used in multiple TUs."
ARENE_CPP14_INLINE_VARIABLE(
    elementwise_binary_transform_detail::elementwise_binary_transform_fn,
    elementwise_binary_transform
);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a
}  // namespace linalg
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_LINALG_ELEMENTWISE_BINARY_TRANSFORM_HPP_
